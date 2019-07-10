build:
	em++ main.cc -s WASM=1 -o target/pong.html -O2 -s LEGACY_GL_EMULATION=1 --shell-file base.html

run: build
	emrun --browser=chromium --port=8090 target
