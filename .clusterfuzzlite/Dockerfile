# ============================================================
# .clusterfuzzlite/Dockerfile
# Base image for building the fuzz harnesses under ClusterFuzzLite.
# ============================================================

FROM gcr.io/oss-fuzz-base/base-builder@sha256:3cf598908df31d5bcc8013f9040e7689daef8cbeedd0401789158fb1abd42cf4

COPY . $SRC/DnsResolver
COPY .clusterfuzzlite/build.sh $SRC/

WORKDIR $SRC/DnsResolver

# DnsResolver vendors VectorPro and HashMapPro as git submodules under
# libs/internal/ -- unlike a dependency-free, header-only project, an
# empty checkout of this repo alone isn't enough for build.sh's include
# paths to resolve. If whatever checked out this repo already populated
# submodules, this is a no-op; if not, this fills them in from the
# commit pinned in .gitmodules before build.sh ever runs.
RUN if [ -d .git ]; then git submodule update --init --recursive; fi
