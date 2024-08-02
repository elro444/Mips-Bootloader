#!/usr/bin/env python3
import click
import fastlz

def do_compress(infile, outfile):
    data = infile.read()
    compressed = fastlz.compress(data, level=1)
    outfile.write(compressed)

def do_decompress(infile, outfile):
    data = infile.read()
    decompressed = fastlz.decompress(data)
    outfile.write(decompressed)

@click.command()
@click.argument('method', type=click.Choice(['compress', 'decompress']))
@click.argument('infile', type=click.File(mode='rb'))
@click.argument('outfile', type=click.File(mode='wb', lazy=True))
def main(method, infile, outfile):
    if method == 'compress':
        do_compress(infile, outfile)
    else:
        do_decompress(infile, outfile)

if __name__ == '__main__':
    main()
