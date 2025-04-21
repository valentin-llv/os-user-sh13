# Projet OS user - Sherlock 13 - From scratch

Cette partie s'eloigne un peu du projet originel, toutefois elle me semble beaucoup plus interessante a regarder que le code que j'ai simplement du completer dans la branche principale !

En voyant le projet (et surtout le code) j'ai voulu le re-creer de zero pour apprendre un maximum de chose et surtout reelement pouvoir jouer avec les threads, mutex, pipe en C (chose que l'on n'a jamais eu l'occasion de faire en cours).
En partant avec cette envie j'ai fait des recherches sur comment architecturer un jeu en C (de la maniere classique mais simple) puis j'ai cree ma propre architecture. J'ai surtout travaille sur le client, car il etait plus interessant avec plusieurs threads (input clavier, rendu SDL, socket, ...).

Par manque de temps, et parce que j'ai voulu profiter des mes seules vacances avant septembre prochain ..., je n'ai malheuresement pas eu le temps de finir avant la deadline du projet. Par ailleurs je ne retravaillerai plus sur ce projet.
Pour autant, j'ai appris beaucoup de chose sur la creation d'un projet plus consequent que ceux realise en cours habituellement. Et je compte mettre en oeuvre ces connaissances dans des projet de code embarque que je realiserai cet ete en plus de mon stage.

## Architecture generale 

Le client doit:
- prendre en compte les entrees via le terminal sous forme de minishell
- faire le rendu graphique avec SDL et prendre en compte les evenements souris et clavier lorsque l'utilisateurs a la focus sur la fenetre
- maintenir une connexion socket avec le serveur

D'apres mes recherches, et pour une architecture modulaire et efficace, on decoupe logiquement en 4 threads:
- le thread principal qui gere les autres threads, (si besoin les evenements window et autres peux frequents / important). Si il est bien fait ce thread est plutot `dormant` la majorite du temps et l'os ne lui donne pas la main sauf si besoin
- le thread de rendu SDL
- le thread de socket, qui est dormant et attend de un evenement reseau
- le thread qui attend un input dans le terminal

Pour que les threads puissent communiquer entre eux il est possible d'utiliser des flags et des variables globals (comme dans l'implementation originale du jeu) mais c'est assez primitifs et peut cause des problemes. Par exemple dans le cas de la reception de deux messages socket trop rapidement, le premier message peut etre perdu si le thread principale le ne traite pas assez vite.
Pour cela j'ai fait une construction hybride avec des flags et ressources global ainsi que des pipes pour communiquer entre les threads.
Les pipes ne sont que partiellement implemente actuellement, et servent surtout pour fermer proprement un thread, mais l'idee est la.

Pour les ressources et flag globaux, j'utilise des mutex pour m'assurer qu'il n'y ai pas de conflit entre les threads. De plus, de part le nature des donnes global, j'utilise different mutex pour securiser differentes donnes. Utiliser un seul mutex bloquerai tous le monde inutillement.
J'ai donc aussi fait attention aux `deadlock` qui peux causer un cas critique de bloquage global du program dans le cas ou plusieurs mutex serai en attente dans different threads.

## Conclusion

Refaire le code du client m'a appris beaucoup de chose sur la facon d'aborder un plus gros projet, et sur l'implementation reelle des threads, pipe et mutex. Malgre le fait que je n'ai pas pu finir, cela me permettra de m'en re-servir plus tard dans un autre projet.