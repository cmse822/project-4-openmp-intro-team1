import pandas as pd
import os
import glob

def compare_matrices(serial_path, parallel_paths):
    """
    Compare the serial matrix to each parallel matrix.
    """
    serial_df = pd.read_csv(serial_path)
    
    for parallel_path in parallel_paths:
        parallel_df = pd.read_csv(parallel_path)
        if not serial_df.equals(parallel_df):
            return False
    return True

def main():
    matrix_output_dir = '../CSVs/matrix_outputs'
    
    # Get a list of all serial matrix files
    serial_files = glob.glob(os.path.join(matrix_output_dir, 'serial_output_matrix_*.csv'))
    
    for serial_file in serial_files:
        # Extract matrix size from serial file name
        matrix_size = os.path.basename(serial_file).split('_')[3].split('.')[0]
        
        # Find all parallel output files for the same matrix size
        parallel_files_pattern = os.path.join(matrix_output_dir, f'parallel_output_matrix_{matrix_size}_*.csv')
        parallel_files = glob.glob(parallel_files_pattern)
        
        # Compare the serial matrix against each parallel matrix
        is_identical = compare_matrices(serial_file, parallel_files)
        
        if is_identical:
            print(f"All parallel instances for matrix size {matrix_size} are identical to the serial output.")
        else:
            print(f"Discrepancy found in parallel instances for matrix size {matrix_size} compared to the serial output.")

if __name__ == "__main__":
    main()
