# Projet OS user - Sherlock 13 - From scratch

Cette partie s'éloigne un peu du projet originel, toutefois elle me semble beaucoup plus intéressante à regarder que le code que j'ai simplement dû compléter dans la branche principale !

En voyant le projet (et surtout le code) j'ai voulu le recréer de zéro pour apprendre un maximum de choses et surtout réellement pouvoir jouer avec les threads, mutex, pipes en C (chose que l'on n'a jamais eu l'occasion de faire en cours).
En partant avec cette envie j'ai fait des recherches sur comment architecturer un jeu en C (de la manière classique mais simple) puis j'ai créé ma propre architecture. J'ai surtout travaillé sur le client, car il était plus intéressant avec plusieurs threads (input clavier, rendu SDL, socket, ...).

Par manque de temps, et parce que j'ai voulu profiter de mes seules vacances avant septembre prochain..., je n'ai malheureusement pas eu le temps de finir avant la deadline du projet. Par ailleurs, je ne retravaillerai plus sur ce projet.
Pour autant, j'ai appris beaucoup de choses sur la création d'un projet plus conséquent que ceux réalisés en cours habituellement. Et je compte mettre en œuvre ces connaissances dans des projets de code embarqué que je réaliserai cet été en plus de mon stage.

## Architecture générale

Le client doit :
- prendre en compte les entrées via le terminal sous forme de minishell
- faire le rendu graphique avec SDL et prendre en compte les événements souris et clavier lorsque l'utilisateur a le focus sur la fenêtre
- maintenir une connexion socket avec le serveur

D'après mes recherches, et pour une architecture modulaire et efficace, on découpe logiquement en 4 threads :
- le thread principal qui gère les autres threads, (si besoin les événements window et autres peu fréquents / importants). S'il est bien fait ce thread est plutôt `dormant` la majorité du temps et l'OS ne lui donne pas la main sauf si besoin
- le thread de rendu SDL
- le thread de socket, qui est dormant et attend un événement réseau
- le thread qui attend un input dans le terminal

Pour que les threads puissent communiquer entre eux il est possible d'utiliser des flags et des variables globales (comme dans l'implémentation originale du jeu) mais c'est assez primitif et peut causer des problèmes. Par exemple dans le cas de la réception de deux messages socket trop rapidement, le premier message peut être perdu si le thread principal ne le traite pas assez vite.
Pour cela j'ai fait une construction hybride avec des flags et ressources globales ainsi que des pipes pour communiquer entre les threads.
Les pipes ne sont que partiellement implémentés actuellement, et servent surtout pour fermer proprement un thread, mais l'idée est là.

Pour les ressources et flags globaux, j'utilise des mutex pour m'assurer qu'il n'y ait pas de conflit entre les threads. De plus, de par la nature des données globales, j'utilise différents mutex pour sécuriser différentes données. Utiliser un seul mutex bloquerait tout le monde inutilement.
J'ai donc aussi fait attention aux `deadlocks` qui peuvent causer un cas critique de blocage global du programme dans le cas où plusieurs mutex seraient en attente dans différents threads.

## Conclusion

Refaire le code du client m'a appris beaucoup de choses sur la façon d'aborder un plus gros projet, et sur l'implémentation réelle des threads, pipes et mutex. Malgré le fait que je n'aie pas pu finir, cela me permettra de m'en resservir plus tard dans un autre projet.