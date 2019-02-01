# Main library
add_library(ordgeo
	lib/core/triplets.cpp
	lib/embed/dlib_opt.cpp
	lib/embed/embed.cpp
	lib/embed/embed_cmp_with_soe.cpp
	lib/io/csv.cpp
	lib/io/io.cpp
	lib/io/svmlight.cpp
	lib/linalg/linalg.cpp
	)

# Tool: soe – Embed triplets with Soft Ordinal Embedding
add_executable(soe
	tools/soe/soe.cpp)
target_link_libraries(soe
	ordgeo
	boost_program_options)
