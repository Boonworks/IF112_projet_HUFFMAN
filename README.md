# Projet HUFFMAN

Ce projet implémente plusieurs méthodes de compression et d’analyse d’images au format `.ppm` :

- Compression naïve
- Compression/décompression avec Huffman
- Analyse statistique (entropie, histogramme, longueur moyenne des codes)

---

## Utilisation avec `make`

Toutes les commandes suivantes peuvent être exécutées via `make`.

> **Remarque :** Si l'input n’est pas précisé, le fichier par défaut est `image.ppm`.

---

### Compression/Décompression Naïve

```bash
make naif input='images/arcade.ppm'
```
> Compression puis décompression naïve de `arcade.ppm`.

---

### Statistiques

```bash
make stats input='images/arcade.ppm'
```
> Affiche : l’histogramme de l’image, l’entropie, la longueur moyenne des codes de `arcade.ppm`.

---

### Compression Huffman

```bash
make huffman input='images/arcade.ppm'
```
> Compression de l’image `arcade.ppm`. avec l’algorithme de Huffman.

---

### Décompression Huffman

```bash
make decompress_huffman
```
> Décompression d’une image compressée précédement avec Huffman.

---

### Histogramme Visuel

```bash
make histo_ppm input='images/arcade.ppm'
```
> Génère une image représentant visuellement l’histogramme de `arcade.ppm`.

---

### Nettoyage

```bash
make clean
```
> Supprime tous les fichiers générés, sauf les fichiers d’entrée `.ppm`.
