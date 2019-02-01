/// @file  embed_cmp.cpp
/// @brief Embeds an object collection using oracle comparisons.

#include <boost/program_options.hpp>
#include <ordgeo/ordgeo.hpp>
#include <json.hpp>
#include <algorithm>
#include <chrono>
#include <map>

using Eigen::Index;
using Eigen::MatrixXd;
using namespace boost::program_options;
using nlohmann::json;
using ORDGEO_NAMESPACE::core::CmpOutcome;
using ORDGEO_NAMESPACE::core::CmpConstraint;
using ORDGEO_NAMESPACE::embed::EmbedConfig;
using ORDGEO_NAMESPACE::embed::EmbedResult;
using ORDGEO_NAMESPACE::io::loadCsvMatrix;
using ORDGEO_NAMESPACE::io::openSink;
using ORDGEO_NAMESPACE::io::openSource;
using ORDGEO_NAMESPACE::io::saveCsvMatrix;
using ORDGEO_NAMESPACE::io::saveEmbedding;
using ORDGEO_NAMESPACE::linalg::embeddingFromKernelSVD;
using std::cerr;
using std::cout;
using std::endl;
using std::map;
using std::max;
using std::string;
using std::stringstream;
using std::vector;

/// Main program.
int run_soe(
	string cmp,
	size_t nDim,
	string initPos,
	double margin,
	double minDelta,
	size_t maxIter,
	size_t bestOf,
	bool saveAll,
	double bestTol,
	bool verbose,
	string out,
	string outstats) {
	json results;
	results["args"] = {
		{"cmp", cmp},
		{"nDim", nDim},
		{"initPos", initPos},
		{"margin", margin},
		{"minDelta", minDelta},
		{"maxIter", maxIter},
		{"bestOf", bestOf},
		{"saveAll", saveAll},
		{"bestTol", bestTol},
		{"verbose", verbose},
		{"out", out},
		{"outstats", outstats},
	};
	EmbedConfig config;
	config.nDim = nDim;
	config.minDelta = minDelta;
	config.maxIter = maxIter;
	config.verbose = verbose;
	config.margin = margin;

	// Load the triples
	vector<CmpConstraint> cons;
	auto incmp = openSource(cmp);
	if (!incmp || !*incmp) {
		cerr << "Could not load constraints from: " << cmp << endl;
		return 1;
	}
	CmpOutcome con;
	map<size_t,size_t> points; // Map from true ID to temp ID
	vector<size_t> pointIds; // Map from temp ID to true ID
	size_t maxObj = 0;
	while (*incmp >> con) {
		for (size_t pt : {con.a, con.b, con.c}) {
			auto it = points.find(pt);
			if (it == points.end()) {
				points[pt] = pointIds.size();
				pointIds.push_back(pt);
				maxObj = max(maxObj, pt);
			}
		}
		cons.emplace_back(con);
	}
	size_t nObj = pointIds.size();
	cout << "Embedding " << nObj << " points referenced by triples" << endl;
	results["args"]["nObj"] = nObj;
	results["num_triples"] = cons.size();

	// Rename objects as needed to embed only what we have constrained
	if (nObj < maxObj + 1) {
		for (auto& con : cons) {
			con.a = points[con.a];
			con.b = points[con.b];
			con.c = points[con.c];
			con.d = points[con.d];
		}
	} else {
		pointIds.clear();
	}

	// Load the initial positions, if we have an init file
	MatrixXd X0(nObj, nDim);
	if (!initPos.empty()) {
		try {
			auto X = loadCsvMatrix(initPos);
			if (pointIds.empty()) {
				X0 = X;
			} else {
				for (Index ii = 0; ii < X0.rows(); ii++) {
					X0.row(ii) = X.row(points[ii]);
				}
			}
		} catch (std::exception &e) {
			cerr << "Could not load initial positions: " << e.what() << endl;
			return 1;
		}
	}
	points.clear();

	// Save the embedding
	auto save = [&](EmbedResult& res, string pathX) {
		try {
			if (!pointIds.empty()) {
				cout << "Writing " << pathX << endl;
				saveEmbedding(res.X, pointIds, pathX);
			} else {
				cout << "Writing " << pathX << endl;
				saveEmbedding(res.X, pathX);
			}
		} catch (const std::exception& e) {
			cerr << e.what() << endl;
			return;
		}
	};

	// Produce the embedding
	EmbedResult bestRes;
	bestOf = max<size_t>(bestOf, 1);
	bestRes.loss = 1000 + bestTol;
	for (size_t rep = 0; rep < bestOf && (saveAll || bestRes.loss > bestTol); rep++) {
		if (initPos.empty()) {
			X0 = MatrixXd::Random(nObj, nDim);
		}

		cout << "Rep " << rep+1 << " / " << bestOf << ": Embedding " << nObj
			<< " objects into R^" << config.nDim << " using "
			<< cons.size() << " constraints" << endl;
		EmbedResult res;
		auto startTime = std::chrono::steady_clock::now();
		auto startProc = clock();
		res = embedCmpWithSOE(cons, X0, config);
		auto endTime = std::chrono::steady_clock::now();
		auto endProc = clock();
		std::chrono::duration<double> diffTime = (endTime - startTime);
		double durTime = diffTime.count();
		double durProc = (endProc - startProc) / CLOCKS_PER_SEC;

		cout << "Rep " << rep+1 << " / " << bestOf << ": Duration: " << durProc
			<< " CPU, " << durTime << " wall" << endl;
		cout << "Rep " << rep+1 << " / " << bestOf << ": Objective: "
			<< res.loss << endl;
		if (rep == 0 || bestRes.loss > res.loss) {
			bestRes = res;
			results["elapsed_perf_counter"] = durProc;
			results["elapsed_process_time"] = durTime;
			results["loss"] = res.loss;
		}

		if (saveAll) {
			stringstream outX;
			outX << out << ".try_" << rep+1 << ".csv.gz";
			save(res, outX.str());
		}
	}
	save(bestRes, out);

	// Save our statistics
	if (!outstats.empty()) {
		cout << "Writing " << outstats << endl;
		auto fstats = openSink(outstats);
		if (!fstats || !fstats->good()) {
			cerr << "Could not open statistics output file" << endl;
			return 1;
		}
		*fstats << std::setw(2) << results << endl;
	}

	cout << "Done" << endl;
	return 0;
}

/// Program entry point.
int main(int argc, char * argv[]) {
	try {
		options_description desc{"soe options"};
		desc.add_options()
			("help,h", "Show this usage message")
			("cmp", value<string>(), "Path to a file containing comparisons to embed (*.csv)")
			("ndim", value<size_t>(), "Embedding dimensionality")
			("init_pos", value<string>()->default_value(""), "Initialize from the specified positions instead of at random (*.csv)")
			("margin", value<double>()->default_value(0), "Constraint margin")
			("min_delta", value<double>()->default_value(1e-12), "Min loss objective delta to stop")
			("max_iter", value<size_t>()->default_value(1000), "Max iteration count")
			("best_of", value<size_t>()->default_value(1), "Return best of n runs")
			("best_tol", value<double>()->default_value(0), "Stop when a run achieves this score")
			("save_all", value<bool>()->default_value(false), "Save intermediate embeddings")
			("verbose", value<bool>()->default_value(false), "Verbose output")
			("out", value<string>(), "Output path for the embedding (*.csv)")
			("outstats", value<string>()->default_value(""), "Output path for statistics (*.json)")
			;

		variables_map vm;
		store(parse_command_line(argc, argv, desc), vm);
		notify(vm);

		bool valid = true;
		if (vm.count("help")) {
			cout << desc << endl;
			valid = false;
		}
		if (!vm.count("cmp")) {
			cerr << "--cmp is required" << endl;
			valid = false;
		}
		if (!vm.count("ndim")) {
			cerr << "--ndim is required" << endl;
			valid = false;
		}
		if (!vm.count("out")) {
			cerr << "--out is required" << endl;
			valid = false;
		}
		if (valid) {
			return run_soe(
				vm["cmp"].as<string>(),
				vm["ndim"].as<size_t>(),
				vm["init_pos"].as<string>(),
				vm["margin"].as<double>(),
				vm["min_delta"].as<double>(),
				vm["max_iter"].as<size_t>(),
				vm["best_of"].as<size_t>(),
				vm["save_all"].as<bool>(),
				vm["best_tol"].as<double>(),
				vm["verbose"].as<bool>(),
				vm["out"].as<string>(),
				vm["outstats"].as<string>());
		}
	} catch (const error &ex) {
		cerr << ex.what() << endl;
	}
	return 1;
}
