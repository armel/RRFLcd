# RRFTracker
Suivi temps réel de l'activité du réseau RRF (Réseau des Répéteurs Francophone)

## Principe de fonctionnement

RRFTracker permet de suivre en temps réel l'activité du réseau RRF.

Au repos, si aucune station n'est en émission, RRFTracker affichera sur,

* la première ligne: l'indicatif du dernier noeud étant passé en émission,
* la seconde ligne: le nombre de passages en émission depuis la mise sous tension du RRFTracker.

Si un QSO est en cours, RRFTracker affichera sur,

* la première ligne: l'indicatif du dernier noeud étant passé en émission,
* la seconde ligne: l'indicatif du noeud en cours d'émission.


## Liste des composants

Voici la liste des composants dont vous aurez besoin:

* 1 mini breadboard
* 1 microcontroleur type Nodemcu Lua ESP8266 (ESP-12E)
* 1 écran LCD 16x2
* 3 résistances 330 ohms
* 3 leds (Rouge, Verte et Bleu)
* quelques cables et straps

L'intérêt du Nodemcu ESP8266 est d'embarquer une puce Wifi et permettre, facilement, une connexion au Net. Mais ce projet est facilement adaptable avec d'autres platines de développement (Arduino, etc.) pour peu qu'une connexion au Net soit envisageable. 
 
## Schéma de montage

![alt text](https://github.com/armel/RRFTracker/blob/master/RRFTracker.png)
