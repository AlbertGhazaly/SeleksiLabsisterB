package server;

import java.io.*;
import java.math.BigInteger;
import java.net.*;
import java.security.SecureRandom;
import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.CopyOnWriteArrayList;


public class Server {
    private static final int PORT = 5000;
    private static final ExecutorService executorService = Executors.newFixedThreadPool(10);

    // These constants should match the ones in the Peer class for the Diffie-Hellman exchange
    private static final BigInteger P = new BigInteger("27865061796865039228588824973413834791461540301542223690288728445636268535884654671166462651030645885780321268165804433561781211220160592342029727590889737351747430434959649704326488220501167306106970172213996380444597273989429658199992194319406021460137974059716414231269614492313635647772615359516962380854415054377024969562045476120397746016799999546520792460028013718774731015439994853590947913294333838040817308213756360689376348887288537778780747974907009687349954163226702905358109974611538596642809487304998431357204292496624642077913739358784118780221893965395473475767672649675081385098589934421037081745599");
    private static final BigInteger G = new BigInteger("21879600352686724075144052683818620248795354631854346375216652956941866807905313635525982267506615542206707288489980495343499474587350783200653870766389739937849885435227148275880786340637266308954210192933459362516250498347013709181574478013851916560238845351712200541677377751667215616075437781980488447063814878384429359210989138229208846024470390173055424409343746291298496762930924995226076527296123002515739025860712340258783540274822060282288767305122930481091095325178655335986007138967249759170268360188709322368787759819544275154823595664890733690845567882880497829598758734290054670542731841539314842261883");
    private static Map<String, String> credentials;
    private static Map<String, String> ipList;
    private static Map<String, String> userList;
    private static List<List<Integer>> friendGraf;
    private static Integer nSize;
    public static void main(String[] args) {
        try {
            // Load user credentials from CSV
            CSVReader reader = new CSVReader();
            credentials = new ConcurrentHashMap<>(reader.readData("credential.csv"));
            userList = new ConcurrentHashMap<>(reader.readData("user.csv"));
            for (Map.Entry<String, String> entry : userList.entrySet()) {
                    System.out.println("Username: " + entry.getKey() + ", Password: " + entry.getValue());
                }
            friendGraf = new CopyOnWriteArrayList<>(reader.readMatrixFromFile("friend.csv"));
            nSize = friendGraf.get(0).size();
            ipList = new ConcurrentHashMap<>();
                try (ServerSocket serverSocket = new ServerSocket(PORT)) {
                System.out.println("Server listening on port " + PORT);

                while (true) {
                    Socket clientSocket = serverSocket.accept();
                    executorService.submit(new ClientHandler(clientSocket));
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private static class ClientHandler implements Runnable {
        private final Socket clientSocket;
        // private final Map<String, String> credentials;
        private String username;
        boolean isLogged = false;
        boolean input_section = false;
        boolean login_id = false;
        boolean login_pass = false;
        boolean register_id = false;
        boolean register_pass = false;
        boolean add = false;
        boolean chat = false;

        public ClientHandler(Socket clientSocket) {
            this.clientSocket = clientSocket;
           
        }

        @Override
        public void run() {
            try (BufferedReader in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
                PrintWriter out = new PrintWriter(clientSocket.getOutputStream(), true)) {

                // Generate server's private and public keys
                BigInteger serverPrivateKey = new BigInteger(256, new SecureRandom());
                BigInteger serverPublicKey = G.modPow(serverPrivateKey, P);

                // Receive client's public key
                BigInteger clientPublicKey = new BigInteger(in.readLine());

                // Send server's public key to the client
                out.println(serverPublicKey.toString());

                // Compute the shared secret
                BigInteger sharedSecret = clientPublicKey.modPow(serverPrivateKey, P);
                byte[] otpKey = sharedSecret.toByteArray();
                otpKey = Arrays.copyOf(otpKey, 16); // Ensure key is 16 bytes long

                // Read messages from the client
                String message;
               

                while ((message = in.readLine()) != null) {
                    message = otpDecrypt(message, otpKey);
                    if (message.equalsIgnoreCase("exit")) {

                        out.println(otpEncrypt("You disconnected.", otpKey));
                        System.out.println("Client Disconnected");
                        synchronized (ipList){
                            ipList.remove(username);
                        }
                        break;

                    }
                    if ((message.equalsIgnoreCase("login") && !input_section)){

                        login_id = true;
                        input_section = true;

                    }else if ((message.equalsIgnoreCase("register") && !input_section)){

                        register_id = true;
                        out.println(otpEncrypt("Enter your username: ", otpKey));
                        input_section = true;
                        
                    }else if ((message.equalsIgnoreCase("add") && !input_section && isLogged)){

                        input_section = true;
                        add = true;

                    }else if ((message.equalsIgnoreCase("chat") && !input_section && isLogged)){

                        input_section = true;
                        chat = true;

                    }else if ((message.equalsIgnoreCase("logout") && !input_section && isLogged)){
                        
                        synchronized (ipList){
                            ipList.remove(username);
                        }
                        username = null;
                        isLogged = false;
                        input_section = false;
                        login_id = false;
                        login_pass = false;
                        register_id = false;
                        register_pass = false;
                        add = false;
                        chat = false;
                        out.println(otpEncrypt("You Logged out !", otpKey));

                    }else{

                        if (register_id){

                            if (isUserTHere(message, credentials)){

                                input_section = false;
                                out.println(otpEncrypt("Username is already used, try again ! ", otpKey));

                            }else{

                                synchronized (credentials){
                                    credentials.put(message, "");
                                }
                                username = message;
                                register_pass = true;
                                out.println(otpEncrypt("Enter your password: ", otpKey));

                            }
                            register_id = false;
                           
                        }else if (register_pass){

                            synchronized (credentials){
                                credentials.put(username, message);
                            }
                            register_id = false;
                            isLogged = true;
                            input_section = false;
                            out.println(otpEncrypt("Registered and Logged in Successfully, welcome "+username+" !", otpKey));

                            synchronized (friendGraf){

                                for (int i =0;i< nSize;i++){
                                    friendGraf.get(i).add(0);
                                }

                                nSize += 1;
                                List<Integer> row = new CopyOnWriteArrayList<>();
                                for (int i=0;i< nSize;i++){
                                    row.add(0);
                                }
                                friendGraf.add(row);
                            }

                            synchronized (userList){
                                userList.put(nSize.toString(),username);
                            }

                            synchronized (ipList){
                                ipList.put(username, (clientSocket.getInetAddress()).getHostAddress());
                            }

                        }else if (login_id){

                            if (isUserTHere(message, credentials)){

                                login_pass = true;
                                out.println(otpEncrypt("Enter your password: ", otpKey));

                            }else{

                                input_section = false;
                                out.println(otpEncrypt("Username False, try again ! ", otpKey));

                            }
                            login_id = false;

                        }else if (login_pass){

                            boolean valid = false;
                            synchronized (credentials){
                                if (isPassTHere(message, credentials,username)){
                                    valid = true;
                                }
                            }                            
                            if (valid){

                                out.println(otpEncrypt("Logged in Successfully, welcome "+username+" !", otpKey));
                                isLogged = true;

                                synchronized (ipList){
                                    ipList.put(username, (clientSocket.getInetAddress()).getHostAddress());
                                }

                            }else{

                                out.println(otpEncrypt("Password False, try again ! ", otpKey));
                                username = "";

                            }
                            login_pass = false;
                            input_section = false;   

                        }else if (add){

                            if (isUserTHere(message, credentials)){

                                int idTeman= 0;
                                int idKita = 0;
                                for (Map.Entry<String, String> entry : userList.entrySet()) {

                                    if (entry.getValue().equals(message)){
                                        idTeman = Integer.valueOf(entry.getKey());
                                    }
                                    if (entry.getValue().equals(username)){
                                        idKita = Integer.valueOf(entry.getKey());
                                    }

                                }

                                synchronized (friendGraf){
                                    friendGraf.get(idKita).set(idTeman,1);
                                }
                                out.println(otpEncrypt("Teman "+message +" berhasil ditambah !",otpKey));

                            }
                            
                        }else if (chat){

                            if (isUserTHere(message, credentials) ){

                                int idTeman= 0;
                                int idKita = 0;
                                for (Map.Entry<String, String> entry : userList.entrySet()) {

                                    if (entry.getValue().equals(message)){
                                        idTeman = Integer.valueOf(entry.getKey());
                                    }
                                    if (entry.getValue().equals(username)){
                                        idKita = Integer.valueOf(entry.getKey());
                                    }
                                }
                                if (friendGraf.get(idKita).get(idTeman)==1){

                                    out.println(otpEncrypt(ipList.get(message),otpKey));

                                    synchronized (ipList){
                                        ipList.remove(username);
                                    }
                                    out.println(otpEncrypt("exit",otpKey));

                                    break;
                                }
                                out.println(otpEncrypt("Not a friend !",otpKey));

                            }
                            out.println(otpEncrypt("User not found !",otpKey));

                        }
                    }

                   
                }
            } catch (IOException e) {
                e.printStackTrace();
            } finally {
                try {
                    clientSocket.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
        private boolean isUserTHere(String message, Map<String, String> credentials){

            for (Map.Entry<String, String> entry : credentials.entrySet()) {
                
                if (entry.getKey().equals(message)){
                    return true;
                }
            }
            return false;
        }
        private boolean isPassTHere(String message, Map<String, String> credentials, String usr){

            if (credentials.get(usr).equals(message)){
                return true;
            }
            return false;
        }
        // Encrypt a message using OTP
        private String otpEncrypt(String message, byte[] key) {

            byte[] messageBytes = message.getBytes();
            byte[] encryptedBytes = new byte[messageBytes.length];

            for (int i = 0; i < messageBytes.length; i++) {
                encryptedBytes[i] = (byte) (messageBytes[i] ^ key[i % key.length]);
            }

            return Base64.getEncoder().encodeToString(encryptedBytes);
        }

        // Decrypt a message using OTP
        private String otpDecrypt(String encryptedMessage, byte[] key) {

            byte[] encryptedBytes = Base64.getDecoder().decode(encryptedMessage);
            byte[] decryptedBytes = new byte[encryptedBytes.length];

            for (int i = 0; i < encryptedBytes.length; i++) {
                decryptedBytes[i] = (byte) (encryptedBytes[i] ^ key[i % key.length]);
            }

            return new String(decryptedBytes);
        }
    }
}
