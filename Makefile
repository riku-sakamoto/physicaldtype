
C_FILES := $(wildcard src/**/*.c src/**/*.h)


.PHONY: format
format:
	clang-format -i $(C_FILES)
	uv run ruff format
	uv run ruff check --fix

.PHONY: lint
lint:
	clang-format --dry-run --Werror $(C_FILES)
	uv run ruff check --output-format=full
	uv run ruff format --diff

# .PHONY: test
# test:
# 	pytest

.PHONY: build
build:
	uv build --wheel

