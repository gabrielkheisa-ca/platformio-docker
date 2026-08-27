# ==========================================
# STAGE 1: Builder Environment & Compile
# ==========================================
FROM python:3.12-slim AS builder

ARG TARGETARCH
# Default build environment (d1_mini_ota or d1_mini_usb)
ARG PIO_ENV=d1_mini_ota

ENV PIP_NO_CACHE_DIR=yes \
    PYTHONUNBUFFERED=1 \
    PLATFORMIO_CORE_DIR=/root/.platformio

# Install system dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    git \
    build-essential \
    curl \
    && rm -rf /var/lib/apt/lists/*

# Install PlatformIO Core
RUN pip install -U platformio

# Pre-cache Espressif8266 platform
RUN if [ "$TARGETARCH" = "arm64" ]; then \
        PLATFORMIO_SYSTEM_TYPE=linux_aarch64 pio platform install espressif8266; \
    else \
        pio platform install espressif8266; \
    fi

WORKDIR /workspace

# Copy project source files into the container
COPY . /workspace

# Accept Wi-Fi credentials as build arguments and set them as environment variables
ARG WIFI_SSID
ARG WIFI_PASS
ENV WIFI_SSID=${WIFI_SSID} \
    WIFI_PASS=${WIFI_PASS}

# Compile firmware for the specified environment
RUN pio run -e ${PIO_ENV}

# ==========================================
# STAGE 2: Expose Output Binary
# ==========================================
FROM scratch AS export

ARG PIO_ENV=d1_mini_ota

# Copy all compiled microcontroller binaries (bin, elf, hex, etc.) to the output
COPY --from=builder /workspace/.pio/build/${PIO_ENV}/firmware.* /