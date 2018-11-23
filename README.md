# RRFTracker
Suivi temps réel de l'activité du réseau [RRF](https://f5nlg.wordpress.com/2015/12/28/nouveau-reseau-french-repeater-network/) (Réseau des Répéteurs Francophone).

## Principe de fonctionnement

RRFTracker permet de suivre en temps réel l'activité du réseau RRF.

Au repos, si aucune station n'est en émission, RRFTracker affichera sur,

* la première ligne: l'indicatif du dernier noeud étant passé en émission,
* la seconde ligne: le nombre de passages en émission depuis la mise sous tension du RRFTracker.

Si un QSO est en cours, RRFTracker affichera sur,

* la première ligne: l'indicatif du dernier noeud étant passé en émission,
* la seconde ligne: l'indicatif du noeud en cours d'émission.

Une video du fonctionnement est visible sur [Youtube](https://www.youtube.com/watch?v=3QY_RpRebKc) ;)

Concernant les LEDs de status,

* la LED bleu s'allume à chaque requête HTTP permettant de connaitre l'état du RRF,
* la LED rouge s'allume si une émission est en court,
* la LED verte s'allume si aucune émission.


## Liste des composants

Voici la liste des composants dont vous aurez besoin:

* 1 mini breadboard
* 1 microcontroleur type Nodemcu Lua ESP8266 (ESP-12E)
* 1 écran LCD 16x2
* 3 résistances 330 ohms
* 3 LEDs (rouge, verte et bleu)
* quelques cables et straps

L'intérêt du Nodemcu ESP8266 est d'embarquer une puce Wifi et permettre, facilement, une connexion au Net. Mais ce projet est facilement adaptable avec d'autres platines de développement (Arduino, etc.) pour peu qu'une connexion au Net soit envisageable. 

Enfin, le choix des couleurs pour les LEDs est totalement arbitraire. Du reste, elles sont surtout là pour faire joli. Maintenant tous projets dignes de ce nom se doivent d'avoir au moins une LED qui s'allume... ;)
 
## Schéma de montage

![alt text](https://github.com/armel/RRFTracker/blob/master/RRFTracker.png)
