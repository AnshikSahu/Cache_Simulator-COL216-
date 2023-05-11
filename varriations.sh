#!/bin/bash

# Define the parameter variations
param1_vals=("8")
param2_vals=("512")
param3_vals=("1")
param4_vals=("16384")
param5_vals=("1")
param6_vals=("trace1.txt" "trace2.txt" "trace3.txt" "trace4.txt" "trace5.txt" "trace6.txt" "trace7.txt" "trace8.txt")

# Loop over all parameter variations
for p1 in "${param1_vals[@]}"; do
  for p2 in "${param2_vals[@]}"; do
    for p3 in "${param3_vals[@]}"; do
      for p4 in "${param4_vals[@]}"; do
        for p5 in "${param5_vals[@]}"; do
          # Create the output folder for the current parameter combination
          output_folder="${p1}_${p2}_${p3}_${p4}_${p5}"
          mkdir -p "$output_folder"
          chmod 777 "$output_folder"
          
          for p6 in "${param6_vals[@]}"; do
            # Build the command with the current parameter values
            command="./cache_simulate $p1 $p2 $p3 $p4 $p5 memory_trace_files/$p6"
            
            # Generate the output file name
            output_file="${p6}"
            
            # Run the command and redirect the output to the output file
            echo "Running command: $command"
            echo "Writing output to: $output_folder/$output_file"
            touch "$output_folder/$output_file"
            chmod 777 "$output_folder/$output_file"
            $command >> "$output_folder/$output_file"
          done
        done
      done
    done
  done
done
