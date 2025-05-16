# Projet IF112 – Compression d’images avec Huffman

Ce projet, implémente en C un système de compression d’images au format PPM, basé sur le codage de Huffman.

Il permet également de comparer ce codage avec une version naïve, d’analyser statistiquement une image, et de générer des représentations visuelles (histogrammes).

---

## Fonctionnalités principales

-  **Création et manipulation d’images** au format `.ppm`
-  **Compression naïve** sur 8 bits fixes (sans réel gain)
-  **Analyse statistique** : histogramme, entropie, longueur moyenne des codes
-  **Compression et décompression avec Huffman** :
  - Construction dynamique d’un arbre de Huffman à partir des fréquences
  - Génération de la table de codage optimale
  - Écriture/lecture d’un format personnalisé `.hppm` avec entête + table + données compressées

---

## Structure

Le projet repose sur un unique fichier source `projet.c` qui regroupe toutes les fonctions.  
La compilation est modulée par un `Makefile` permettant d’exécuter différentes parties du projet grâce à des macros de compilation.

---
## Installation

Clonez le dépôt :

   ```bash
   git clone https://github.com/Boonworks/IF112_projet_HUFFMAN
  ```

---

## Utilisation via `make`

> **Remarque** : Le fichier d’entrée par défaut est `image.ppm`. Vous pouvez en spécifier un autre avec `input=...`.


### ➤ Compression/Décompression aïve

```bash
make naif input='images/arcade.ppm'
```
> Compression puis décompression naïve de `arcade.ppm`.

---

###  ➤ Statistiques

```bash
make stats input='images/arcade.ppm'
```
> Affiche : l’histogramme de l’image, l’entropie, la longueur moyenne des codes de `arcade.ppm`.

> Génère une image représentant visuellement l’histogramme de `arcade.ppm`.

---

###  ➤ Compression Huffman

```bash
make huffman input='images/arcade.ppm'
```
> Compression de l’image `arcade.ppm`. avec l’algorithme de Huffman.

---

###  ➤ Décompression Huffman

```bash
make decompress_huffman
```
> Décompression d’une image compressée précédement avec Huffman.

---

###  ➤ Nettoyage

```bash
make clean
```
> Supprime tous les fichiers générés, sauf les fichiers d’entrée `.ppm`.

---

## Remarques

  Le Makefile utilise des macros pour activer différentes parties du programme via des blocs conditionnels dans projet.c.

  Le format HPPM est un format personnalisé contenant l’en-tête, la table de codage, et les données compressées.

  Le projet est conçu pour fonctionner sous Windows; le Makefile adapte les commandes de suppression en conséquence, mais les commandes pour Linux sont commentées dans le Makefile.


---
Thomas Besson

Projet encadré par R. Giraud – ENSEIRB-MATMECA – IF112 2024/2025