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

Culling skips sprites outside the camera. Sprites are spread over a world 2x wider
and 2x taller than the screen, so about a quarter are visible.

| Sprites | Drawn | Unculled median | Culled median | Unculled p95 | Culled p95 |
|--------:|------:|----------------:|--------------:|-------------:|-----------:|
| 10,000 | 2,596 | 0.69 ms | 0.58 ms | 3.01 ms | 3.33 ms |
| 50,000 | 12,826 | 3.98 ms | 1.34 ms | 5.91 ms | 3.03 ms |

Apple M3, Release build, vsync off. 60 warmup frames discarded, then median and
p95 of the next 500.

```sh
cmake -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release --parallel
./bin/vertex_SpriteBench 10000
./bin/vertex_SpriteBench 50000 2
```

## License

See [LICENSE](LICENSE).
