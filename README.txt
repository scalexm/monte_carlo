Dans tout ce qui suit, ainsi que dans les fichiers source, les mots "article", "section" et
"proposition" se réfèrent à l'article "Computation of VaR and CVaR using stochastic
approximations and unconstrained importance sampling", Pagès et al., 2009.


*** Structure du code ***

Les sources des deux algorithmes de calcul de la V@R et CV@R se trouvent dans le répertoire `src`.
Dans `estimate.hpp`, on trouvera l'API publique. Dans le répertoire `detail`, on trouvera les
détails d'implémentation. Tout est documenté directement dans les fichiers source, à l'aide de
commentaires.


*** Exécutables ***

On produit deux exécutables.

    ** `short_put` **

    Cet exécutable est constituté de l'unique fichier source `short_put.cpp`. Il calcule la V@R et
    CV@R pour un portefeuille correspondant à une position courte sur une option de vente de
    strike K = 110 et de maturité T = 1 an. On suppose que le sous-jacent suit une dynamique
    Black-Scholes avec une volatilité sigma = 20% et un prix initial S0 = 100. Le prix P0 à
    laquelle l'option a été vendue est P0 = 10.7. On prend un taux d'intérêt annuel r = 5%. Tous
    ces paramètres sont exactement ceux de l'exemple 1 de la section 5.1 de l'article.

    Pour compiler cet exécutable: `g++ -O2 -std=c++11 short_put.cpp -o short_put`.
    Pour l'exécuter: `./short_put [options] <alpha> <N>`.

    Description des paramètres obligatoires:
    * `alpha` est le niveau de confiance pour les calculs de la V@R et CV@R
    * `N` est le nombre d'itérations à effectuer

    Description des options:

    * `--method <m>`: choix de l'algorithme, `m <- stochastic-gradient` pour l'algorithme
                      de gradient stochastique naïf, `m <- importance-sampling` pour
                      l'algorithme avec importance sampling
    --- Par défaut, on fait `m <- stochastic-gradient`.

    * `--averaging <avg>`: appliquer ou non la moyennisation de Ruppert et Polyak, `avg <- yes`
                           pour l'appliquer, `avg <- no` pour ne pas l'appliquer
    --- Par défaut, on fait `avg <- no`.

    * `--step <exponent> <offset>`: choix du pas gamma, si `exponent` et `offset` sont des valeurs
                                    flottantes alors le pas sera `1/(n^exponent + offset)`
    --- Par défaut, on fait `exponent <- 1.0`, `offset <- 0.0`


    ** `exponential_distribution` **


