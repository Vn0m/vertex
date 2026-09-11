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

## License

See [LICENSE](LICENSE).
