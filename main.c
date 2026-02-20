
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <malloc.h>
#include "ratp.h"
#include "age.h"

/*
SYNTHÈSE DU TP :

Ce code illustre la mise en œuvre progressive de tests unitaires avec CMocka. Il commence par
démontrer la pertinence des assertions spécialisées (comme assert_float_equal) pour gérer
la précision des flottants et faciliter le débogage par rapport aux comparaisons booléennes standards.

L'implémentation intègre ensuite le mécanisme de fixtures (setup/teardown) pour gérer proprement
le cycle de vie des tests. Cela permet d'initialiser et de nettoyer les ressources (mémoire)
automatiquement, garantissant ainsi l'indépendance et la robustesse de chaque exécution.

Enfin, l'exercice aborde l'isolation du code via le mocking. Les dépendances externes sont
simulées (via will_return) pour tester la logique métier de manière isolée, tout en préparant
l'environnement pour l'analyse de la couverture de code (Code Coverage) avec gcov.
 */


/*
Les fonctions de ratp.c ont étés changés pour suivre l'exigence du TP:

"règle de gestion : if age <= 12 le prix du ticket (1,5) est divisé par deux."



Question 1: Il a besoin de l'epsilon pour savoir quel est l'ordre de grandeur des nombres comparés
Question 2: Le assert_equal c'est meilleur parce qu'il nous donne plus d'infomartion du contexte de l'erreur en utilisant
les résultats précis des fonctions:
    [  ERROR   ] --- computePrice(12) == 1.2 -> assert_true
    [  ERROR   ] --- 0.750000 != 0.600000    -> assert_equals
Elle affiche explicitement la valeur attendue et la valeur obtenue en cas d'erreur.

Question 4: Séparer les tests garantit que l'échec d'une assertion n'empêche pas l'exécution des suivantes et permet d'identifier plus vite la cause précise du bug.

Pour la vérification des tests en utilisant GCOV:
        
    LDFLAGS = -lcmocka --coverage -lgcov
    OPTIONS = -Wall -fexceptions -fprofile-arcs -ftest-coverage 
        L> utilisé pour la compilation e pour la liaison

Pour voir les résultats, on a utilisé un ".venv" pour installer gcovr 

    gcovr --html-details result.html 

Pour mocker les fonctions qui ne sont pas prêtes il faut utiliser le mock "will_return". Cela utilise 
la pile pour envoyer les valeurs des fonctions dedans le test dans les cas où elles ne sont pas à jour.

*/

// fonction appellée mais mockée
void recupAge(int *valeur){
    *valeur = (int) mock(); // en utilisant la valuer donnée
}

static const float epsilon = 0.01;

// l'implementation customisé du setup pour passer les paramètres dans le "state"
static int setup_custom(void **state) {
    int *age = malloc(sizeof(int)); // state reçoie les inforamtion de l'age
    *age = 13;
    *state = age;  // en passent l'age au state
    return 0;
}


static int setup(void **state) {
    (void) state;
    printf("setUp");
 return 0;
}

// le teardown est aussi customisé lorsqu'on a utilisé un malloc dedans le "setup_custom" implementation
static int teardown(void **state) {
    free(*state);   // déallocation de memoires
    return 0;
}


// tests communs
static void computePrice_plus12(void **state) {
    (void) state;
    assert_float_equal(computePrice(13), 1.5, epsilon);
}
static void computePrice_moins12(void **state) {
    (void) state;
    assert_float_equal(computePrice(12), 1.5/2, epsilon);
}

// tests faux (cas d'échec) avec assert_true et assert_float_equal
static void faux_true_computePrice_moins12(void **state) {
    (void) state;
    assert_true(computePrice(12) == 1.2); // faux assert_true 
}
static void faux_equals_computePrice_moins12(void **state) {
    (void) state;
    assert_float_equal(computePrice(12), 1.2/2, epsilon); // faux assert_equals
}

// test avec setup
static void computePrice_plus12_setup(void **state) {
    int *age = (int*) *state;      
    assert_float_equal(computePrice(*age), 1.5, epsilon);
}


// test avec mock recupAge
static void computePrice_moins12_mock_recupAge(void **state) {
    (void) state;
    will_return(recupAge, 12);  // mock la valuer pour l'age avec une fonction simulacre 
    assert_float_equal(computePrice(12), 1.5/2, epsilon); 
}

// pour chaque cas de test les résultats ont été analysés, même avant l'utilisation de l'age.h
int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(computePrice_plus12),
        cmocka_unit_test(computePrice_moins12),
        cmocka_unit_test(faux_true_computePrice_moins12),
        cmocka_unit_test(faux_equals_computePrice_moins12),
        cmocka_unit_test_setup(computePrice_plus12_setup, setup_custom),
        cmocka_unit_test(computePrice_moins12_mock_recupAge)
    };
    //cmocka_set_message_output(CM_OUTPUT_XML);
    return cmocka_run_group_tests_name("computePrice tests",tests, setup, teardown);
}
