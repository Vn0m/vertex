# Vertex

A 2D game engine written in C++17 and OpenGL 3.3, plus **Glade**, a small
top-down RPG.

CSCI 49900 capstone, Hunter College, Fall 2026.

## Building

Dependencies are git submodules, so the clone has to be recursive:

```sh
git clone --recursive https://github.com/Vn0m/vertex.git
cd vertex
cmake -B build
cmake --build build --parallel
./bin/glade
```

Already cloned without `--recursive`:

```sh
git submodule update --init --recursive
```

## Layout

```
vertex/   the engine, built as a static library
glade/    the game executable
vendor/   third-party dependencies
```

## Benchmarks

Frame time to draw N sprites, before sprite batching (one draw call per sprite):

| Sprites | Median | p95 |
|--------:|-------:|----:|
| 100 | 0.51 ms | 1.45 ms |
| 1,000 | 1.68 ms | 3.52 ms |
| 5,000 | 7.86 ms | 9.56 ms |
| 10,000 | 16.62 ms | 17.32 ms |

Apple M3, Release build, vsync off. 60 warmup frames discarded, then median and
p95 of the next 500.

```sh
cmake -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release --parallel
./bin/vertex_bench 10000
```

## License

See [LICENSE](LICENSE).
