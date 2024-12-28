# **Benchmark-Superseed**

This project provides Snakemake workflows to process FASTA files using tools available in two directories: `individualXP` and `intersectionXP`.

---

## **Setup and Execution**

### **1. Install dependencies and tools**
Make sure you have Conda installed. Then, run the following command in the project root directory to set up the tools and environment:

```bash
make
```

### **2. Activate the environment**
Activate the Conda environment to access the required tools and Python scripts:

```bash
conda activate benchmark_env
```

---

## **Usage**

### **1. Prepare the `input_files.txt` file**
- Create a file named `input_files.txt` in your desired directory (`individualXP` or `intersectionXP`).
- Add the absolute paths of the FASTA files to be processed, **one path per line**. Example:
  ```plaintext
  /absolute/path/to/file1.fa
  /absolute/path/to/file2.fa
  /absolute/path/to/file3.fa
  ```

### **2. Navigate to the desired directory**
- **For the `individualXP` workflow**:
  ```bash
  cd individualXP
  ```
- **For the `intersectionXP` workflow**:
  ```bash
  cd intersectionXP
  ```

### **3. Run Snakemake**
Execute the workflow with the following command:
```bash
snakemake --cores 1
```

---

## **Results**
The results will be generated in the `results` directory within the respective folder (`individualXP` or `intersectionXP`).

---
