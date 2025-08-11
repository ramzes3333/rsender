FROM rockylinux:8 AS build

# Base build deps
RUN dnf -y install epel-release && \
    dnf -y install gcc gcc-c++ make git cmake curl xz unzip \
                   ncurses-devel gpm-devel && \
    dnf clean all

# Build & install nlohmann_json (CMake package)
WORKDIR /opt
RUN curl -fsSLo /tmp/json.tar.xz \
      https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz && \
    mkdir -p /opt/nlohmann && \
    tar -xJf /tmp/json.tar.xz -C /opt/nlohmann --strip-components=1 && \
    cmake -S /opt/nlohmann -B /opt/nlohmann/build -DJSON_BuildTests=OFF && \
    cmake --build /opt/nlohmann/build -j"$(nproc)" && \
    cmake --install /opt/nlohmann/build

# Build & install Turbo Vision
RUN git clone --depth=1 https://github.com/magiblot/tvision.git
WORKDIR /opt/tvision
RUN cmake -B build -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build -j"$(nproc)" && \
    cmake --install build

# Sources
WORKDIR /src
ADD script /src/script
ADD ui /src/ui
ADD util /src/util
ADD CMakeLists.txt /src/
ADD Const.h /src/
ADD main.cpp /src/

# Static C++ runtime (for -static-libstdc++ -static-libgcc)
RUN dnf -y install dnf-plugins-core epel-release && \
    dnf config-manager --set-enabled powertools && \
    dnf -y install libstdc++-static && \
    dnf clean all

# Configure, build, install to /out
RUN cmake -B build -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_RPATH="\$ORIGIN/lib" \
    -DCMAKE_BUILD_WITH_INSTALL_RPATH=ON
RUN cmake --build build -j"$(nproc)"
RUN cmake --install build --prefix /out && ls -R /out

# ----- minimal runtime image -----
FROM rockylinux:8 AS out
WORKDIR /app
COPY --from=build /out/ ./
ENTRYPOINT ["/app/bin/rsender"]