FROM alpine:3.12.1 as build

RUN apk add --no-cache gcc g++ make cmake libldap openldap-dev

WORKDIR /twmailer
COPY ./src /twmailer/src
COPY ./include /twmailer/include
COPY ./CMakeLists.txt /twmailer

WORKDIR /twmailer/build
RUN cmake .. && make -j

FROM alpine:3.12.1 as runtime
RUN apk add --no-cache libstdc++ libldap

WORKDIR /twmailer

COPY --from=build /twmailer/build/server .
COPY --from=build /twmailer/build/client .