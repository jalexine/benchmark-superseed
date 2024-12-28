# Directories
TOOLS_DIR = external
BIN_DIR = bin
KMC_DIR = $(TOOLS_DIR)/kmc
SEQKIT_BIN = $(BIN_DIR)/seqkit
GGCAT_BIN = $(BIN_DIR)/ggcat
SSHASH_BIN = $(BIN_DIR)/sshash
SKANI_BIN = $(BIN_DIR)/skani

# Conda environment
CONDA_ENV = benchmark_env
ENV_YML = environment.yml

.PHONY: all install tools kmc ggcat seqkit sshash skani clean conda_env cargo

all: install

install: conda_env tools
	@echo "Environment and tools installed. You can 'cd individualXP/ or intersectionXP/' now run 'snakemake --cores 1'."

tools: | $(BIN_DIR) kmc ggcat seqkit sshash skani

# Create the bin directory if it doesn't exist
$(BIN_DIR):
	@echo "Creating bin directory..."
	mkdir -p $(BIN_DIR)

kmc:
	@echo "Installing KMC..."
	git submodule update --init --recursive
	$(MAKE) -C $(KMC_DIR)
	@echo "Moving KMC executables to $(BIN_DIR)..."
	mv $(KMC_DIR)/bin/* $(BIN_DIR)

ggcat:
	@echo "Installing GGCAT via Conda..."
	conda install -y -c conda-forge -c bioconda ggcat
	@echo "Linking GGCAT executable to $(BIN_DIR)..."
	ln -sf $(shell which ggcat) $(GGCAT_BIN)

seqkit:
	@echo "Installing SeqKit..."
	curl -LO https://github.com/shenwei356/seqkit/releases/download/v2.3.1/seqkit_linux_amd64.tar.gz
	tar -xzf seqkit_linux_amd64.tar.gz
	mv seqkit $(SEQKIT_BIN)
	rm -f seqkit_linux_amd64.tar.gz
	@echo "SeqKit installed at $(SEQKIT_BIN)"

sshash: install_rust check_rust
	@echo "Building SSHash..."
	cd $(TOOLS_DIR)/sshash && mkdir -p build && cd build && \
		cmake .. -DSSHASH_USE_MAX_KMER_LENGTH_63=On && make -j
	mv $(TOOLS_DIR)/sshash/build/sshash $(SSHASH_BIN)
	@echo "SSHash built at $(SSHASH_BIN)"


skani:
	@echo "Installing Skani..."
	cd $(TOOLS_DIR)/skani && cargo build --release
	mv $(TOOLS_DIR)/skani/target/release/skani $(SKANI_BIN)
	@echo "Skani built at $(SKANI_BIN)"

# Install Conda environment
conda_env:
	@echo "Setting up Conda environment..."
	conda env create -f $(ENV_YML) || conda env update -f $(ENV_YML)
	$(MAKE) cargo
	@echo "Conda environment $(CONDA_ENV) ready!"

# Install Rust and cargo
install_rust:
	@echo "Installing Rust and cargo..."
ifeq ($(shell uname), Darwin)
	# macOS
	brew install rust || true
else
	# Linux
	curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
	export PATH="$HOME/.cargo/bin:$(PATH)"  # Ajout au PATH
endif
	@echo "Rust and cargo installed successfully!"

check_rust:
	@echo "Checking Rust installation..."
	cargo --version || (echo "Cargo is not installed! Check your installation." && exit 1)

clean:
	@echo "Cleaning up tools..."
	rm -rf $(BIN_DIR)/*
	rm -rf $(TOOLS_DIR)/kmc/bin $(TOOLS_DIR)/sshash/build $(TOOLS_DIR)/skani/target
	conda remove -y ggcat || true
	conda env remove -n $(CONDA_ENV) || true
