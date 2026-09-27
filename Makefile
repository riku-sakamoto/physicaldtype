
C_FILES := $(wildcard src/**/*.c src/**/*.h)


.PHONY: format
format:
	clang-format -i $(C_FILES)
	uv run ruff format .

.PHONY: lint
lint:
	clang-format --dry-run --Werror $(C_FILES)
	uv run ruff check .

# .PHONY: test
# test:
# 	pytest

.PHONY: build
build:
	uv build --wheel

