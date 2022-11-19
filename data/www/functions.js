import { rand } from './utils.js';
import FnEditor from './fn.editor.js';

const Modal = {
  components: {
    FnEditor,
  },
  props: {
    fn: [Boolean, Object],
  },
  data() {
    return {
      name: '',
      editor: []
    }
  },
  emits: ['close', 'update'],
  watch: {
    fn: async function() { // Modal open
      if (this.fn.file) {
        const response = await fetch(this.fn.file);
        if (response.ok) {
          ({ name: this.name, functions: this.editor } = await response.json());
        }
      }
    }
  },
  methods: {
    close() {
      this.name = '';
      this.editor = [];

      this.$emit('close');
    },
    async save() {
      const put = async file => {
        const response = await fetch(file, {
          method: 'PUT',
          headers: {
            'Content-Type': 'application/json'
          },
          body: JSON.stringify({
            name: this.name,
            functions: this.editor,
          })
        });

        if (response.ok) {
          this.$emit('update', { file, name: this.name });
          this.close();
        }
      };

      if (this.fn.file) {
        put(this.fn.file);
      } else {
        const file = `/fns/${rand().toString().substring(0, 8)}.json`;
        put(file);
      }
    },
  },
  template: `
  <div>
    <div class="modal d-block">
      <div class="modal-dialog m-0 m-md-auto">
        <form @submit.prevent="save" class="modal-content">
          <div class="modal-header">
            <h5 class="modal-title">Function Editor</h5>
            <button @click="close" type="button" class="btn-close"></button>
          </div>
          <div class="modal-body">
            <div class="row">
              <div class="col-12">
                <div class="form-floating">
                  <input v-model="name" type="text" class="form-control" required maxlength="20" placeholder="Function Set Name" />
                  <label>Function Set Name</label>
                </div>
              </div>
            </div>
            <div class="row">
              <div class="col text-center">
                <FnEditor v-model="editor" class="mt-3" />
              </div>
            </div>
          </div>
          <div class="modal-footer">
            <button @click="close" type="button" class="btn btn-secondary">Close &amp; Discard</button>
            <button type="submit" class="btn btn-primary">Save Changes</button>
          </div>
        </form>
      </div>
    </div>
    <div class="modal-backdrop fade show"></div>
  </div>
  `
};

export default {
  components: {
    Modal,
  },
  props: {
    active: Boolean
  },
  data() {
    return {
      save: false,
      fns: [],
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
      const response = await fetch('/fns');
      this.fns = await response.json();
      this.isLoading = false;
    },
    add() {
      this.save = true;
    },
    edit(fn) {
      this.save = fn;
    },
    async del(fn) {
      if (confirm('Are you sure you want to delete this function set?')) {
        const response = await fetch(fn.file, { method: 'DELETE' });
        if (response.ok) {
          this.fns = this.fns.filter(({ file }) => file !== fn.file);
        }
      }
    },
    update(fn) {
      const existing = this.fns.findIndex(({ file }) => file === fn.file);
      if (existing !== -1) { // Change existing
        this.fns[existing].name = fn.name;
      } else { // Add new
        this.fns.push(fn);
      }
    },
    upload({ target }) {
      for (const file of target.files) {
        const reader = new FileReader();
        reader.onload = async () => {
          const json = JSON.parse(reader.result);
          if (json.name && json.functions) { // Basic validation
            const fns = `/fns/${rand().toString().substring(0, 8)}.json`;
            const response = await fetch(fns, {
              method: 'PUT',
              headers: {
                'Content-Type': 'application/json'
              },
              body: reader.result,
            });
            
            if (response.ok) {
              this.update({ file: fns, name: json.name })
            }
          }
        };
        reader.readAsText(file);
      }
      target.value = '';
    },
  },
  template: `
  <div>
    <div class="row">
      <div class="col-md-5 mx-auto">
        <div class="row">
          <div class="col d-flex align-items-center">
            <div class="position-relative px-3">
              <h1 class="display-5">
                Functions
              </h1>
              <span class="position-absolute top-0 start-100 translate-middle badge rounded-pill bg-primary mt-2">
                {{ fns.length }}
              </span>
            </div>
          </div>
          <div class="col-auto">
            <label class="btn btn-link text-primary p-0">
              <svg width="32" height="32" fill="currentColor">
                <use xlink:href="bs.icons.svg#upload"/>
              </svg>
              <input @change="upload" type="file" accept="application/json" class="d-none" />
            </label>
            <button @click="add" type="button" class="btn btn-link text-success p-0">
              <svg width="64" height="64" fill="currentColor">
                <use xlink:href="bs.icons.svg#plus-lg"/>
              </svg>
            </button>
          </div>
        </div>
      </div>
    </div>
    <div class="row">
      <div class="col-md-5 mx-auto">
        <hr class="bg-secondary" />
      </div>
    </div>
    <div class="row">
      <div class="col-md-5 mx-auto">
        <ul :class="{ loading: isLoading }" class="list-group list-group-flush">
          <li v-for="fn of fns" :key="fn.file" class="list-group-item">
            <div class="row">
              <div class="col">{{ fn.name }}</div>
              <div class="col-auto d-flex flex-nowrap">
                <button @click="edit(fn)" class="btn btn-link p-0 d-flex align-items-center">
                  <svg width="16" height="16" fill="currentColor">
                    <use xlink:href="bs.icons.svg#pencil"/>
                  </svg>
                </button>
                &nbsp;&nbsp;
                <button @click="del(fn)" class="btn btn-link p-0 d-flex align-items-center">
                  <svg width="16" height="16" fill="currentColor">
                    <use xlink:href="bs.icons.svg#trash"/>
                  </svg>
                </button>
              </div>
            </div>
          </li>
          <li v-if="!fns.length" class="list-group-item d-flex align-items-center">
            No function sets, click the <span @click="add" class="text-success h1" style="cursor: pointer;">+</span> to add one
          </li>
        </ul>
      </div>
    </div>
    <Modal :fn="save" v-show="save" @close="save = false" @update="update" />
  </div>
  `
}
