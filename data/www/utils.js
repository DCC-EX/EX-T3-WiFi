export function rand() {
  const rand = new Uint32Array(1);
  crypto.getRandomValues(rand);

  return rand[0];
};

export function elementIndex(el) {
  return [...el.parentElement.children].indexOf(el);
};
