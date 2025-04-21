# Projet OS user - Sherlock 13

Code du projet de jeu multijoueur en ligne avec socket pour le cours d'OS user.

## File structure

Le code est separe en 2 parties:
- le code et les ressources du client
- le code du serveur

## Run

Pour lancer le projet, il faut lancer le serveur puis 4 clients

### Running the server

Pour lancer le server

Aller dans le dossier du server:
```sh
cd server
```

Puis make pour compiler le code
```sh
make
```

Enfin la commande pour lancer le serveur:
```sh
./dist/sh13-server <port>
```

Exemple:
```sh
./dist/sh13-server 3000
```

### Running the client

Pour lancer un client (il faut en lancer 4 pour jouer une partie)

Aller dans le dossier du client:
```sh
cd client
```

Compiler les sources:
```sh
make
```

Excuter un client:
```sh
./dist/sh13-client <server-adr> <server-port> <client-adr> <client-port> <player-name>
```

Exemple:
```sh
./dist/sh13-client localhost 3000 localhost 4000 client1
```

**Note**: Pour executer plusieurs client ne pas oublier de changer le `client-port` pour ne pas créer de conflits