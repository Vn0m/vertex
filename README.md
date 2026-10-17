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

Frame time to draw N sprites with one texture. Unbatched issues one draw call per
sprite, batched issues one draw call per frame.

| Sprites | Unbatched median | Batched median | Unbatched p95 | Batched p95 |
|--------:|-----------------:|---------------:|--------------:|------------:|
| 100 | 0.51 ms | 0.61 ms | 1.45 ms | 3.68 ms |
| 1,000 | 1.68 ms | 0.54 ms | 3.52 ms | 1.57 ms |
| 5,000 | 7.86 ms | 0.57 ms | 9.56 ms | 1.49 ms |
| 10,000 | 16.62 ms | 0.65 ms | 17.32 ms | 1.47 ms |

Apple M3, Release build, vsync off. 60 warmup frames discarded, then median and
p95 of the next 500.

```sh
cmake -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release --parallel
./bin/vertex_bench 10000
```

## License

See [LICENSE](LICENSE).
