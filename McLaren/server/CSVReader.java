package server;
import java.io.*;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.CopyOnWriteArrayList;

public class CSVReader {

    public static Map<String,String> readData(String csv) {
        String csvFile = csv;  // Path to your CSV file
        String line;
        String csvDelimiter = ",";  // Delimiter used in the CSV file

        Map<String, String> mapResult = new HashMap<>();

        try (BufferedReader br = new BufferedReader(new FileReader(csvFile))) {
            while ((line = br.readLine()) != null) {
                // Split line by the delimiter to extract username and password
                String[] parts = line.split(csvDelimiter);
                if (parts.length == 2) {
                    String username = parts[0];
                    String password = parts[1];
                    // Store username and password in the map
                    mapResult.put(username, password);
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        return mapResult;
        // Print the map to verify the contents
        // for (Map.Entry<String, String> entry : credentialsMap.entrySet()) {
        //     System.out.println("Username: " + entry.getKey() + ", Password: " + entry.getValue());
        // }
    }
    
   public static List<List<Integer>> readMatrixFromFile(String fileName) {
        List<List<Integer>> matrix = new CopyOnWriteArrayList<>();

        try (BufferedReader reader = new BufferedReader(new FileReader(fileName))) {
            // Read the first line to get the size of the matrix
            String line = reader.readLine();
            int n = Integer.parseInt(line.trim()); // Parse the size

            // Read the matrix
            for (int i = 0; i < n; i++) {
                line = reader.readLine(); // Read each line of the matrix
                String[] values = line.trim().split("\\s+"); // Split the line by spaces

                List<Integer> row = new CopyOnWriteArrayList<>();
                for (int j = 0; j < n; j++) {
                    row.add(Integer.parseInt(values[j])); // Parse each value into the matrix
                }
                matrix.add(row);
            }

        } catch (IOException e) {
            System.err.println("Error reading the file: " + e.getMessage());
        } catch (NumberFormatException e) {
            System.err.println("Invalid number format: " + e.getMessage());
        }

        return matrix;
    }
}
