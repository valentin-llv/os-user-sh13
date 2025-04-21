# Rapport projet OS user

Le rapport pour ce projet sera sous la forme d'un fichier markdown. Par manque d'intérêt, et pour ne pas réexpliquer le fonctionnement du jeu en lui-même (ce qui n'aurait aucun intérêt) ni chaque ligne de code déjà écrite, je vais garder ce rapport assez court et le concentrer sur l'essentiel.

**Note :** Une partie plus intéressante de ce projet est disponible dans la branche `from-scratch` que je vous invite à regarder.

## Architecture générale

Le jeu se joue en ligne, avec 4 clients et un serveur. Le serveur gère les règles du jeu, calcule l'état courant du jeu et le retransmet aux clients. Les clients affichent l'état du jeu et prennent en compte les entrées du jeu pour envoyer les actions.

### Client

Le client se décompose en 2 threads :
- le thread de socket pour la gestion et le maintien de la connexion socket avec le serveur
- le thread principal qui s'occupe du rendu et de prendre en compte les clics de la souris

### Le serveur

Le serveur n'est construit que sur un seul thread et gère la connexion avec les 4 clients.

## Communication

Pour communiquer, le client et le serveur échangent des messages en respectant un protocole défini à l'avance. Exemple de message :
```sh
L player1 player2 player3 player4
```

Le premier caractère sert d'identificateur de l'action à laquelle correspond le message puis les éléments suivants sont les paramètres. Tous les messages adoptent cette forme, pour le sens client -> serveur et le sens serveur -> client.

Le client ne peut envoyer des messages qu'au serveur.
Le serveur, lui, peut envoyer un message à un seul client ou à plusieurs (broadcast) en cas de besoin.

## Concepts vus en cours

Le projet ne met en œuvre que 2 des concepts vus en cours : les threads et les mutex.

Côté client, les threads permettent de découpler le rendu de la gestion de la connexion. On s'assure de lire les messages le plus vite possible sans attendre le rendu d'une frame. À l'inverse, on peut rendre les frames sans s'occuper d'attendre un message et potentiellement perdre du temps.

Les deux threads utilisent des variables globales pour communiquer, le `gbuffer` pour stocker les données en attente depuis le thread socket vers le thread principal.

Pour éviter les conflits et informer le thread principal d'une ressource disponible, deux variables sont utilisées. La variable `synchro` pour signaler la disponibilité d'une ressource et la variable `mutex` qui est un mutex de `pthread` pour sécuriser l'utilisation des ressources par plusieurs threads.

## Complétion du code

Le code ayant déjà été majoritairement écrit, la complétion des morceaux manquants a été assez rapide et consistait surtout en l'appel des fonctions d'envoi des messages.

## Conclusion

Ce projet nous a permis de voir la mise en œuvre d'une architecture client / serveur pour un programme et de voir une utilisation des threads et des mutex en C (sans toutefois les implémenter nous-mêmes), ainsi que la création d'un protocole personnalisé pour les messages client / serveur et transmettre les actions de jeu.