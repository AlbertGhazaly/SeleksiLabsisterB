import java.io.*;
import java.net.*;

public class PeerToPeer {
    private static final int PORT = 5000; // Port yang digunakan

    public static void main(String[] args) {
        try {
            // Jalankan server untuk mendengarkan koneksi
            new Thread(() -> startServer()).start();

            // Jalankan client untuk mengirim pesan
            startClient();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    // Fungsi untuk memulai server
    private static void startServer() {
        try {
            ServerSocket serverSocket = new ServerSocket(PORT);
            System.out.println("Peer server listening on port " + PORT);

            while (true) {
                Socket socket = serverSocket.accept();
                BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                String message;
                while ((message = in.readLine()) != null) {
                    if (message.equalsIgnoreCase("exit")) {
                        System.out.println("Connection closed.");
                        break;
                    }
                    System.out.println("Received: " + message);
                }
                in.close();
                socket.close();
                if (message.equalsIgnoreCase("exit")) {
                    break; // Exit the server loop if "exit" is received
                }
            }
            serverSocket.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    // Fungsi untuk memulai client
    private static void startClient() {
        try {
            BufferedReader userInput = new BufferedReader(new InputStreamReader(System.in));
            System.out.println("Enter the address of the peer to connect to:");
            String peerAddress = userInput.readLine();

            Socket socket = new Socket(peerAddress, PORT);
            PrintWriter out = new PrintWriter(socket.getOutputStream(), true);

            String message;
            while (true) {
                try {
                    Thread.sleep(300);
                } catch (InterruptedException e) {
                    // Handle the exception if the thread is interrupted
                    e.printStackTrace();
                }
                System.out.println("Enter your message:");
                message = userInput.readLine();
                out.println(message);
                if (message.equalsIgnoreCase("exit")) {
                    System.out.println("Exiting...");
                    break; // Exit the client loop if "exit" is entered
                }
            }

            out.close();
            socket.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
