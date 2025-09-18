# Options Doodle

## General Usage

`brightnessctl <operation> [options...]`

## Operations

- `get`
- `set`

## Options

- `-q, --quiet`
- `-p, --pretend`
- `-m, --machine-readable`
- `-d, --device=DEVICE`
- `-c, --class=CLASS`
- `-h, --help`
- `-v, --version`
- `-s, --save`

**For set**

`brightnessctl set [options] [value]

- `-n, --min-value=n`
- `-e, --exponent[=K]`
- `-r, --restore`

value is required if restore is not set

**For get**

- `-l, --list`

