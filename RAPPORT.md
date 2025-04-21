# Rapport projet OS user

Le rapport pour ce projet sera sous la forme d'un fichier markdown. Par manque d'interet, et pour ne pas re-epliquer le fonctionnement du jeu en lui meme (ce qui n'aurait auncun interet) ni chaque ligne de code deja ecrite, je vais garder ce rapport assez court et le concentrer sur l'essentiel.

**Note :** Une partie plus interressante de ce projet est disponible dans la branche `from-scratch` que je vous invite a regarder.

## Architecture general

Le jeu se joue en ligne, avec 4 client et un serveur. Le  serveur gere les regles du jeu, calcule l'etat courant du jeu et le retransmet aux clients. Les clients affichent l'etat du jeu et prennent en comtpe les entrees du jeu pour envoyer les actions.

### Client

Le client se decomposer en 2 threads:
- le thread de socket pour la gestion et le maintien de la connexion socket avec le serveur
- le thread principal qui s'occupe du rendu et de prendre en compte les clics de la souris

### Le serveur

Le serveur n'est construit que sur un seul thread et gere la connexion avec les 4 clients.

## Communication

Pour communiquer le client et le serveur echange des message en respectant un protocol defini en avance. Exemple de message:
```sh
L player1 player2 player3 player4
```

Le premier caractere sert d'identificateur de l'action a laquelle correspond le message puis les elements suivant sont les parametres. Tous les message adopte cette forme, pour le sens client -> server et le sens server -> client.

Le client ne peux uniquement envoyer des messages au serveur.
Le serveur lui peux envoyer un message a un seul client ou a plusieurs (broadcast) en cas de besoin.

## Concepts vu en cours

Le projet ne met en oeuvre que 2 des concepts vu en cours: les threads et les mutexs.

Cote client les threads permettent de decoupler le rendu de la gestion de la connexion. On s'assure de lire les messages le plus vite sans attendre le rendu d'une frame. A l'inverse on peut rendre les frames sens s'oocuper d'attendre un message et potentiellement perdre du temps.

Les deux threads utilisent des variables globales pour communiquer, le `gbuffer` pour stocker les donnees en attente depuis le threads socket vers le threads principal.

Pour eviter les conflit et informer le thread principal d'une ressource disponible deux variables sont utilise. La variable `synchro` pour signaler la disponibilite d'une ressources et la variable `mutex` qui est un mutex de `pthread` pour securiser l'utilisation des ressources par plusieurs threads.

## Completion du code

Le code ayant deja ete majoritairement ecrit, la completion des morceaux manquant a ete assez rapide et consistait surtout en l'appel des fonction d'envoie des message.

## Conclusion 

Ce projet nous a permis de voir la mise en ouevre d'une architecture client / serveur pour un programme et de voir une utilisation des threads et des mutex en C (sans toutefois les implementer nous-memes), ainsi qued la creation d'un protocol personnaliser pour les message client / serveurs et transmettre les actions de jeu.