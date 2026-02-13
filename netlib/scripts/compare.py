import highspy
import sys

def load_and_solve_mps(file_path):
    # Create HiGHS instance
    highs = highspy.Highs()
    highs.setOptionValue("output_flag", False)
    highs.setOptionValue("log_to_console", False)
    
    # Read the MPS file
    status = highs.readModel(file_path)
    if status != highspy.HighsStatus.kOk:
        raise ValueError(f"Failed to read MPS file {status}: {file_path}")
    
    # Solve the model
    status = highs.run()
    if status != highspy.HighsStatus.kOk:
        raise ValueError(f"Failed to solve MPS file: {file_path}")
    
    # Get the objective value
    obj_value = highs.getObjectiveValue()
    return obj_value

def compare_mps_solutions(file1, file2, tolerance=1e-6):
    val1 = load_and_solve_mps(file1)
    val2 = load_and_solve_mps(file2)
    
    if abs(val1 - val2) <= tolerance * abs(val1 + val2) / 2:
        print(f"The solutions match: {val1} approx {val2}")
        return True
    else:
        print(f"The solutions differ: {val1} vs {val2}")
        return False

if __name__ == "__main__":
    import sys
    if len(sys.argv) != 3:
        print("Usage: python compare_mps_highs.py file1.mps file2.mps")
        sys.exit(1)
    
    file1, file2 = sys.argv[1], sys.argv[2]
    if not compare_mps_solutions(file1, file2):
        sys.exit(1)
