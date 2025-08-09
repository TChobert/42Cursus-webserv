const jokes = [
  "Pourquoi les développeurs n'aiment-ils pas la nature ? Trop de bugs.",
  "Qu'est-ce qu’un développeur dit en sortant de soirée ? git commit -m 'Soûl'",
  "Combien de devs faut-il pour changer une ampoule ? Aucun, c’est un problème hardware.",
  "Pourquoi le JavaScript aime les cookies ? Parce qu’il peut les manger côté client.",
  "Un serveur rentre dans un bar... et plante.",
  "404 – blague non trouvée."
];

function newJoke() {
  const joke = jokes[Math.floor(Math.random() * jokes.length)];
  document.getElementById("joke").textContent = joke;
}
