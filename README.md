# Projet OS user - Sherlock 13

Code du projet de jeu multijoueur en ligne avec socket pour le cours d'OS user.

**Note :** en plus de la branche main, une autre branche, `from-scratch`, est disponible, je vous invite à aller la visiter.

## Structure des fichiers

Le code est séparé en 2 parties :
- le code et les ressources du client
- le code du serveur

## Exécution

Pour lancer le projet, il faut lancer le serveur puis 4 clients

### Lancer le serveur

Pour lancer le serveur :

Aller dans le dossier du serveur :
```sh
cd server
```

Puis faire make pour compiler le code :
```sh
make
```

Enfin, la commande pour lancer le serveur :
```sh
./dist/sh13-server <port>
```

Exemple :
```sh
./dist/sh13-server 3000
```

### Lancer le client

Pour lancer un client (il faut en lancer 4 pour jouer une partie) :

Aller dans le dossier du client :
```sh
cd client
```

Compiler les sources :
```sh
make
```

Exécuter un client :
```sh
./dist/sh13-client <server-adr> <server-port> <client-adr> <client-port> <player-name>
```

Exemple :
```sh
./dist/sh13-client localhost 3000 localhost 4000 client1
```

**Note :** Pour exécuter plusieurs clients, ne pas oublier de changer le `client-port` pour ne pas créer de conflits.