# Toolchain image for GitLab CI (.gitlab-ci.yml), so build and docs jobs do
# not run apt-get on every pipeline.
#
# The image is tagged with the first 16 hex digits of this file's sha256. After
# editing this file, update TOOLCHAIN_TAG in .gitlab-ci.yml to
#   sha256sum test/Dockerfile | cut -c1-16
# The toolchain-image job rebuilds the image when this file changes, and fails
# if TOOLCHAIN_TAG does not match.

FROM ubuntu:22.04

# gcc g++ clang cmake ninja-build binutils git: build jobs.
# python3-sphinx: docs job. The docs job also needs gcc/g++: project() enables
# C/CXX, so configure fails without a compiler even though `docs` is a custom
# target.
RUN apt-get update -qq \
 && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
      gcc g++ clang cmake ninja-build binutils git python3-sphinx \
 && rm -rf /var/lib/apt/lists/*
