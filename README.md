# ordgeo

Ordinal Geometry Toolkit: ordinal embedding, triplets, and such.

This project includes:

* libordgeo, a C++ library for ordinal geometry and embeddings.
* The ordgeo Python library (a swig wrapper around libordgeo).
* Command line tools, in C++, providing easy executable access to libordgeo.

Instructions for installing and using each of these components are provided in
the sections below.

# Command line tools

The library and all command line tools can be built with the following commands.
```
git clone git@github.com:jesand/ordgeo
mkdir ordgeo/release
cd ordgeo/release
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

## Docker

If you have docker installed, you can instead build the tools in a docker image
using the following commands.

```
cd ordgeo/docker
docker build --tag=ordgeo .
```

You can then get an interactive shell on the docker image like so.
```
docker run -it ordgeo:latest bash
```

## soe: Embed triplets with Soft Ordinal Embedding

Embeds triplet comparisons using the Soft Ordinal Embedding algorithm from:

Y. Terada and U. von Luxburg, “Local ordinal embedding,” presented at the Proceedings of the 31st International Conference on Machine Learning, 2014.

The following command line, executed from the ordgeo folder, will run for up to 1000 iterations and save the embedding to `ordgeo/examples/soe_result.csv.gz`.
```
release/src/soe --cmp examples/triplets.csv.gz --ndim 3 --out examples/soe_result.csv.gz
```

The output should resemble:
```
Embedding 1000 points referenced by triples
Rep 1 / 1: Embedding 1000 objects into R^3 using 159584 constraints
Rep 1 / 1: Duration: 5 CPU, 5.01348 wall
Rep 1 / 1: Objective: 5.74657e-13
Writing examples/soe_result.csv.gz
Done
```

The resulting embedding file is a gzipped csv file with 4 columns. The first
column uniquely identifies an embedded object, and the other three provide
the location within R^3 where the point was embedded.

The command line arguments can be listed with `soe --help`. They are:

* `-h` or `--help`: Print usage information.
* `--cmp <path>`: Path to a .csv or .csv.gz file containing triplets to constrain the embedding.
* `--ndim <int>`: Number of dimensions for embedding.
* `--init_pos <path>`: Path to a .csv or .csv.gz file containing initial point positions (otherwise random positions from the unit hypercube are used).
* `--margin <float>`: The SOE margin parameter (otherwise a reasonable default is chosen).
* `--min_delta <float>`: Set the minimum improvement required to continue training.
* `--max_iter <int>`: Set the maximum number of training iterations.
* `--best_of <int>`: Repeat training several times and pick the result with minimum SOE loss.
* `--best_tol <float>`: Stop repetitions early if this loss or better is achieved.
* `--save_all <bool>`: If true, all `--best_of` embeddings are saved.
* `--verbose <bool>`: Enable extra output (such as loss per iteration).
* `--out <path>`: Save the final embedding to this path (.csv or .csv.gz).
* `--outstats <path>`: Save some extra statistics, such as run time, to this path (.json).

# ordgeo Python package

* TODO: Installing Python package
* TODO: Python documentation
* TODO: Python examples

# libordgeo

* TODO: Installing libordgeo
* TODO: API documentation
* TODO: API Examples
