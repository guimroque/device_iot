#include <pgmspace.h>

#define SECRET
#define THINGNAME "ESP32_TESTE"                                       //change name of thing in AWS

const char WIFI_SSID[] = "Marcia.";                                        //change wifi name
const char WIFI_PASSWORD[] = "naotemsenha@2022";                                    //change wifi password
const char AWS_IOT_ENDPOINT[] = "alimdirxho7qi-ats.iot.us-east-1.amazonaws.com";        //change AWS endpoint to things

// Amazon Root CA 1
static const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

// Device Certificate                                               //change this
static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWjCCAkKgAwIBAgIVAJjR14zOsEu6q3n+NTPVDU2PBpBoMA0GCSqGSIb3DQEB
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMjAyMDgyMzQz
NTJaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDTs75nT7oCnFH7LzG3
gfYo5jauHHB1uem3XGoBxf3IljmPbHWrq9/1ZpGcCb/FKLK6h0I8YkqwqU7RdiLo
dcuApslfQ/u3JX5aoSje5G0WA9cchXDc0iWTUOGNIrGA7uEbgiaz5SuUiACHbqxj
78qVnhaRYHa1k8qy1aiE/rOBXYe+oCI7hDjiBpDyyyFiXIb7DRlJpZYHv641vnyL
pIQFLVpkLtSr7cpCk/Mxg8FRTIeM9Latae1TFPM6RW8E9D7ffA7P20dMm3gFUH0X
kq5MveCTOTKqw1qag2F4UcA9a+SEMJIpXwK8tRZzC8DPhrZiHYPFAomZgXJ1db49
2xmjAgMBAAGjYDBeMB8GA1UdIwQYMBaAFJtGXoe/eA7vJ+CzbklumOgajs8YMB0G
A1UdDgQWBBR43h9vkfX8hH8gIwINKM0qZacksTAMBgNVHRMBAf8EAjAAMA4GA1Ud
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAiSB+MGkX+Y8p1lTghg0mw2pp
egQzCybnQ4sshVYfD4SwI9qor4WjNHC60K4P4e+imo9JqWATtk8AI/UejN+47/Ce
njJFets4jh+odceIUEaKXH4exTi3+E5EAg/ONIgAmEfX4ZCzwJZNT7x61KpnklMk
w/orV5bqmIVPjQHhWQKuP975KQ7/CW3D7tAcdMdHlE1wcuR0kjPnj7cFvPJrHFCt
egyKbtWUs5VYG2xwDqgVC6/fgTs1PpGn7Bdwwc1YryM4Xw1QWf9knrusf+0RkyK5
+3Ha8fKRgRCHnj8A1dDrRsbIH5rtGyjdu/HtknvB+Ooq1Rb7/9szKfewFexM0A==
-----END CERTIFICATE-----
)KEY";

// Device Private Key                                               //change this
static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEA07O+Z0+6ApxR+y8xt4H2KOY2rhxwdbnpt1xqAcX9yJY5j2x1
q6vf9WaRnAm/xSiyuodCPGJKsKlO0XYi6HXLgKbJX0P7tyV+WqEo3uRtFgPXHIVw
3NIlk1DhjSKxgO7hG4Ims+UrlIgAh26sY+/KlZ4WkWB2tZPKstWohP6zgV2HvqAi
O4Q44gaQ8sshYlyG+w0ZSaWWB7+uNb58i6SEBS1aZC7Uq+3KQpPzMYPBUUyHjPS2
rWntUxTzOkVvBPQ+33wOz9tHTJt4BVB9F5KuTL3gkzkyqsNamoNheFHAPWvkhDCS
KV8CvLUWcwvAz4a2Yh2DxQKJmYFydXW+PdsZowIDAQABAoIBAQCzkuk0qx0mb8De
p2atuXC7uqOxRvBh/CRy49IL1Q1zo3IJ5b9esnlTqQBkcqSXsfEBZhBaNioS9lqk
NGhh9OF9LHzt8TXIYeZvK/sGLo8VuIY0bECTHzK+ZkIcvMwCiR6rfdgkioVQBZkl
SEy0rMeT6DhOcpvfZBDxZ2j9/yr21HuDrVUp0G3HQmHSMs7ImGFsNr7UXD1jYtGv
Z97wOvV7OmaBfUsb+1JwzvCuvPoMiXsi0pWGDuQk+6D/cxU+frzqnyAZ9nqW+7+f
GVamT3Kk8tD974sboh1/TxHPx7J6V4TvceScK/4Zxk+LMrK7g/LrjNAbfFOr8rnL
Yxez3Q8ZAoGBAOtdCeFLhXNIAlGo5nfq0hwwQ13JMvA5gqFZWM8vNFVN7guth29B
ire4AbbdCra1UqEB5hzi9ZOJlYKeVU0BU7RFA+toZyEELWkduI9WD4uzgKE7o4U8
OGQd8UkwhWxWIBmKzjqoCZ50ydVXgx3pGr6pHbI+XR2gzmZRCie9fKVvAoGBAOZD
mqSdBegxEjM/kzGkMfoTbp3RKRLiAc5IHcD1syMFz2kJWj6q1TYn/4VO0JR93tVg
9Ekr8kQZC/RIA9be5t43QfTXiJylC5zvR42alKJ00NEhXLn5pZtslXqgebm+iDXC
17OZ/tj4PFWNO0o/PuL/YTLcnbd+RHQVJVLNeP0NAoGAH+FJTk/5coUFc2QqW4He
Fd1fsptnH+zaSdqdUer17rwQi1pk5uf8zyfIa6mDIv5qg86LCcygMOzI6Jljunt8
iAV1I3TPy0QSlpNlBV1F28ljmfNHEev3Q3Be12gQGn2L+Ym4p29NZj4xfIlPBEUn
zz1+/T6/RB0c9/TwKDPdZ1cCgYATLFPnGxAP2m4qiEOoYYSrzwxp5a2m3dzKoWSg
2ZSV+PjhRR8Xkr/4J3J2Vi61/pkcaVtN8HPWycMeDxUKwjq1DCLY0U2VxYRnB+GM
X++6a0Gr+DwxXCE3BN6ZfF1GYyASYev2D3+R73GOqhjiwMtquVsNkiMr0T6/77bQ
4z0MzQKBgQCp9jbbzVhBPB325UNUFkYGdh9Fczwq1a2Sk4hKqIp96l4v5mk6AnvI
P8saKc/H9nHJ/cu3T9g81MXRZSc6+6REmkSW1HktaqBSLcVjpKY/H+oYshvsy4Zi
UvKQ6wIDFQ/6xLsKTnswq294wVjDk+up0kAkMa+lACHpyTnn0lvLiw==
-----END RSA PRIVATE KEY-----
)KEY";
