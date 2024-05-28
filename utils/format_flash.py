#!/usr/bin/env python3
from dataclasses import dataclass
from pathlib import Path
import click

@dataclass(frozen=True)
class Partition:
    name: str
    start: int
    data: bytes

    @property
    def size(self):
        return len(self.data)


def is_colliding(a: Partition, b: Partition) -> bool:
    if not a.size or not b.size:
        return False

    if a.size > b.size:
        bigger, smaller = a, b
    else:
        bigger, smaller = b, a

    for point in (smaller.start, smaller.start + smaller.size):
        if bigger.start <= point < bigger.start + bigger.size:
            return True

    return False



@click.command()
@click.argument('output')
@click.option('--partition', 'partitions', multiple=True,
              help='Input partitions in the format of "file_path:flash_offset"')
def main(output, partitions):
    output = Path(output)
    parsed_partitions: list[Partition] = []
    for partition_parameter in partitions:
        filepath, offset = partition_parameter.split(':')
        filepath = Path(filepath)
        offset = int(offset, base=0)

        if not filepath.is_file():
            raise ValueError(f"File {filepath} does not exist!")
        if offset < 0:
            raise ValueError(f"Partition {partition_parameter!r} has negative offset {offset}!")

        with open(filepath, 'rb') as f:
            data = f.read()

        new_partition = Partition(partition_parameter, offset, data)
        for partition in parsed_partitions:
            if is_colliding(new_partition, partition):
                raise ValueError(
                    f"Partition {new_partition.name!r} collides with partition {partition.name}!"
                )

        parsed_partitions.append(new_partition)

    with open(output, 'wb') as f:
        for partition in parsed_partitions:
            f.seek(partition.start)
            f.write(partition.data)


if __name__ == '__main__':
    main()
