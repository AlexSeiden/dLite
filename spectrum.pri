# The following macros allow certain features and debugging output
# to be disabled / enabled at compile time.

# Debug output from spectrum calculation
#DEFINES += LOG_SPECTRUMANALYSER

# Debug output from engine
#DEFINES += LOG_ENGINE

# Disable calculation of level
#DEFINES += DISABLE_LEVEL

# Perform spectrum analysis calculation in a separate thread
DEFINES += SPECTRUM_ANALYSER_SEPARATE_THREAD
