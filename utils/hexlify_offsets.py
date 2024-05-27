#!/usr/bin/env python
import re
import click

def hexlify_offset(match):
    offset = match.group(1)
    return ',' + hex(int(offset))

@click.command()
@click.argument('filename')
def main(filename):
    with open(filename, 'r') as f:
        data = f.read()
    lines = data.split('\n')
    newlines = []
    for line in lines:
        newlines.append(re.sub(r',(-?[1-9]\d*)', hexlify_offset, line))
    newdata = '\n'.join(newlines)
    with open(filename, 'w') as f:
        f.write(newdata)

if __name__ == '__main__':
    main()
