export default {
  props: {
    active: Boolean
  },
  data() {
    return {
      icons: [],
      isLoading: false,
    }
  },
  watch: {
    active: {
      handler(value) {
        if (value) {
          this.load();
        }
      },
      immediate: true
    }
  },
  methods: {
    async load() {
      this.isLoading = true;
      const response = await fetch('/icons');
      this.icons = (await response.json()).map((icon, idx) => {
        return {
          file: icon,
          load: idx === 0 || this.icons.findIndex(({ file }) => file === icon) !== -1
        }
      });
      this.isLoading = false;
    },
    upload({ target }) {
      for (const file of target.files) {
        const img = '/icons/' + file.name;
        const reader = new FileReader();
        reader.onload = async () => {
          const view = new DataView(reader.result);
          // check if the bmp is equal/less than 30x30 and 24bit
          if (view.getUint32(18, true) <= 30 && view.getUint32(22, true) <= 30 && view.getUint16(28, true) == 24) {
            const response = await fetch(img, {
              method: 'PUT',
              headers: {
                'Content-Type': 'application/octet-stream',
              },
              body: new Blob([reader.result]),
            });
            
            if (response.ok) {
              this.icons.push({
                file: img,
                load: true
              });
            }
          }
        };
        reader.readAsArrayBuffer(file);
      }
      target.value = '';
    },
    async del(icon) {
      if (confirm('Are you sure you want to delete this icon?')) {
        const response = await fetch(icon.file, { method: 'DELETE' });
        if (response.ok) {
          this.icons = this.icons.filter(({ file }) => file !== icon.file);
        }
      }
    },
    next(index) { // Lazy load icons to not overload the ESP32
      if (this.icons[index]) {
        this.icons[index].load = true;
      }
    }
  },
  template: `
  <div>
    <div class="row">
      <div class="col-md-5 mx-auto">
        <div class="row">
          <div class="col d-flex align-items-center">
            <div class="position-relative px-3">
              <h1 class="display-5">
                Icons
              </h1>
              <span class="position-absolute top-0 start-100 translate-middle badge rounded-pill bg-primary mt-2">
                {{ icons.length }}
              </span>
            </div>
          </div>
          <div class="col-auto">
            <label class="btn btn-link text-success p-0">
              <svg width="64" height="64" fill="currentColor">
                <use xlink:href="bs.icons.svg#plus-lg"/>
              </svg>
              <input @change="upload" type="file" accept="image/bmp" class="d-none" />
            </label>
          </div>
        </div>
      </div>
    </div>
    <div class="row">
      <div class="col-md-5 mx-auto">
        <hr class="bg-secondary" />
      </div>
    </div>
    <div class="row mb-3">
      <div class="col-md-5 mx-auto">
        <div class="card">
          <div class="card-header">
            <svg width="16" height="16" fill="currentColor">
              <use xlink:href="bs.icons.svg#info"/>
            </svg>
            Note
          </div>
          <div class="card-body">
            <p class="card-text">Icons must be 24bit bitmaps with max dimensions of 30x30</p>
          </div>
        </div>
      </div>
    </div>
    <div class="row">
      <div class="col-md-5 mx-auto">
        <ul :class="{ loading: isLoading }" class="list-group list-group-flush">
          <li v-for="(icon, idx) of icons" :key="icon.file" class="list-group-item">
            <div class="row">
              <div class="col-2 text-center me-auto">
                <img :src="icon.load ? icon.file : null" :data-src="icon.file" @load="next(idx + 1)" />
              </div>
              <div class="col d-flex align-items-center">
                {{ icon.file.match(/([^\\/]*).bmp$/i)[1] }}
              </div>
              <div class="col-auto d-flex align-items-center">
                <button @click="del(icon)" type="button" class="btn btn-link p-0 d-flex align-items-center">
                  <svg width="16" height="16" fill="currentColor">
                    <use xlink:href="bs.icons.svg#trash"/>
                  </svg>
                </button>
              </div>
            </div>
          </li>
          <li v-if="!icons.length" class="list-group-item d-flex align-items-center">
            No icons, click the <span class="text-success h1">+</span> to add one
          </li>
        </ul>
      </div>
    </div>
  </div>
  `
}
