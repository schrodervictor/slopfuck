FROM gcc:latest AS build
WORKDIR /src
COPY slopfuck.c keywords.h praise.h bookends.h multipliers.h style.h ./
RUN gcc -Wall -Wextra -O2 -std=c99 -o slopfuck slopfuck.c

FROM debian:bookworm-slim
COPY --from=build /src/slopfuck /usr/local/bin/slopfuck
ENTRYPOINT ["slopfuck"]
