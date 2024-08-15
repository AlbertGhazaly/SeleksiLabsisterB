import java.io.*;
import java.math.BigInteger;
import java.net.*;
import java.security.*;
import java.util.*;

public class Peer {

    private static final int PORT = 5000;
    private static final BigInteger P = new BigInteger("27865061796865039228588824973413834791461540301542223690288728445636268535884654671166462651030645885780321268165804433561781211220160592342029727590889737351747430434959649704326488220501167306106970172213996380444597273989429658199992194319406021460137974059716414231269614492313635647772615359516962380854415054377024969562045476120397746016799999546520792460028013718774731015439994853590947913294333838040817308213756360689376348887288537778780747974907009687349954163226702905358109974611538596642809487304998431357204292496624642077913739358784118780221893965395473475767672649675081385098589934421037081745599"); // Prime number
    private static final BigInteger G = new BigInteger("21879600352686724075144052683818620248795354631854346375216652956941866807905313635525982267506615542206707288489980495343499474587350783200653870766389739937849885435227148275880786340637266308954210192933459362516250498347013709181574478013851916560238845351712200541677377751667215616075437781980488447063814878384429359210989138229208846024470390173055424409343746291298496762930924995226076527296123002515739025860712340258783540274822060282288767305122930481091095325178655335986007138967249759170268360188709322368787759819544275154823595664890733690845567882880497829598758734290054670542731841539314842261883");  // Primitive root modulo P
    private static final String ipServer = "192.168.68.55";
    private static String msg;
    public static void main(String[] args) {
        try {
            // Start server in a separate thread
           new Thread(() -> startServer()).start();

           // Start client to send messages
           startClient(ipServer);
        } catch (Exception e) {
           e.printStackTrace();
       }
    }
    // try {
    //     // Start server in a separate thread
    //     new Thread(() -> startServer()).start();

    //     // Start client to send messages
    //     startClient();
    // } catch (Exception e) {
    //     e.printStackTrace();
    // }
    // Function to start the server
    private static void Communicate(String ip){
        try {
            // Start server in a separate thread
           new Thread(() -> startServer()).start();

           // Start client to send messages
           startClient(ip);
        } catch (Exception e) {
           e.printStackTrace();
       }
    }
    private static void startServer() {
        try {
            ServerSocket serverSocket = new ServerSocket(PORT);
            System.out.println("Peer server listening on port " + PORT);

            // Generate server's public key
            BigInteger serverPrivateKey = new BigInteger(256, new Random());
            BigInteger serverPublicKey = G.modPow(serverPrivateKey, P);

            // Wait for connection
            Socket socket = serverSocket.accept();
            BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            PrintWriter out = new PrintWriter(socket.getOutputStream(), true);

            // Receive client's public key
            BigInteger clientPublicKey = new BigInteger(in.readLine());

            // Send server's public key to client
            out.println(serverPublicKey.toString());

            // Compute shared secret
            BigInteger sharedSecret = clientPublicKey.modPow(serverPrivateKey, P);
            byte[] otpKey = sharedSecret.toByteArray();
            otpKey = Arrays.copyOf(otpKey, 16); // Ensure key is 16 bytes long

            String message;
            while ((message = in.readLine()) != null) {
                
                
                // Decrypt the message
                String decryptedMessage = otpDecrypt(message, otpKey);
                msg = decryptedMessage;
                System.out.println("Received: " + decryptedMessage);

                if (message.equalsIgnoreCase("exit")) {
                    System.out.println("Connection closed.");
                    break;
                }
            }

            in.close();
            out.close();
            socket.close();
            serverSocket.close();
            startServer();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    // Function to start the client
    private static void startClient(String ip) {
        try {
            BufferedReader userInput = new BufferedReader(new InputStreamReader(System.in));

            
            System.out.println("Connecting to "+ip+" (Server)...");

            Socket socket = new Socket(ip, PORT);
            PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
            BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            // Generate client's public key
            BigInteger clientPrivateKey = new BigInteger(256, new Random());
            BigInteger clientPublicKey = G.modPow(clientPrivateKey, P);

            // Send client's public key to server
            out.println(clientPublicKey.toString());

            // Receive server's public key
            BigInteger serverPublicKey = new BigInteger(in.readLine());

            // Compute shared secret
            BigInteger sharedSecret = serverPublicKey.modPow(clientPrivateKey, P);
            byte[] otpKey = sharedSecret.toByteArray();
            otpKey = Arrays.copyOf(otpKey, 16); // Ensure key is 16 bytes long

            String message;
            boolean isToChat  = false;
            String PeerIp = null;
            boolean isExit = false;
            while (true) {
                System.out.println("Enter your message:");
                message = userInput.readLine();

                // Encrypt the message
                String encryptedMessage = otpEncrypt(message, otpKey);
                out.println(encryptedMessage);
                System.out.println("Sent message: "+encryptedMessage);
               
                if (message.equals("chat")){
                    isToChat = true;
                }
                
                if (isToChat){
                    isToChat = false;
                    synchronized (msg){
                        if (!(msg.equals("Not a friend !") ||  msg.equals("User not found !"))){
                            PeerIp = msg;        
                            break;
                        }
                    }
                }
                
                if (message.equalsIgnoreCase("exit")) {                   
                        System.out.println("Exiting...");
                        isExit = true;
                        break;
                }
            }

            out.close();
            in.close();
            socket.close();
            if (isExit){
                return;
            }
          
            System.out.println("Connecting to "+PeerIp+"...");
            
            socket = new Socket(PeerIp, PORT);
            out = new PrintWriter(socket.getOutputStream(), true);
            in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            // Generate client's public key
            clientPrivateKey = new BigInteger(256, new Random());
            clientPublicKey = G.modPow(clientPrivateKey, P);

            // Send client's public key to server
            out.println(clientPublicKey.toString());

            // Receive server's public key
            serverPublicKey = new BigInteger(in.readLine());

            // Compute shared secret
            sharedSecret = serverPublicKey.modPow(clientPrivateKey, P);
            otpKey = sharedSecret.toByteArray();
            otpKey = Arrays.copyOf(otpKey, 16); // Ensure key is 16 bytes long

            
            isToChat  = false;
            PeerIp = null;
            while (true) {
                System.out.println("Enter your message:");
                message = userInput.readLine();

                // Encrypt the message
                String encryptedMessage = otpEncrypt(message, otpKey);
                out.println(encryptedMessage);
                // System.out.println("Sent message: "+encryptedMessage);
                if (message.equalsIgnoreCase("exit")) {
                    System.out.println("Exiting...");
                    break;
                }
            }
            out.close();
            in.close();
            socket.close();
            if (isExit){
                return;
            }
            startClient(ipServer);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    // Encrypt a message using OTP
    private static String otpEncrypt(String message, byte[] key) {
        byte[] messageBytes = message.getBytes();
        byte[] encryptedBytes = new byte[messageBytes.length];
        
        for (int i = 0; i < messageBytes.length; i++) {
            encryptedBytes[i] = (byte) (messageBytes[i] ^ key[i % key.length]);
        }

        return Base64.getEncoder().encodeToString(encryptedBytes);
    }

    // Decrypt a message using OTP
    private static String otpDecrypt(String encryptedMessage, byte[] key) {
        byte[] encryptedBytes = Base64.getDecoder().decode(encryptedMessage);
        byte[] decryptedBytes = new byte[encryptedBytes.length];
        
        for (int i = 0; i < encryptedBytes.length; i++) {
            decryptedBytes[i] = (byte) (encryptedBytes[i] ^ key[i % key.length]);
        }

        return new String(decryptedBytes);
    }
}
