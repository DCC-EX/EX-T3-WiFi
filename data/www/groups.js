import { rand, elementIndex } from './utils.js';

const Modal = {
  props: {
    locos: Object,
  },
  data() {
    return {
      address: '',
      locosMutated: [],
    }
  },
  emits: ['close'],
  watch: {
    locos: function(value) {
      this.locosMutated = Array.from(Object.entries(value), ([address, name]) => {
        return {
          address: address,
          name: name,
          selected: false,
        }
      });
    }
  },
  methods: {
    validAddress({ target }) {
      if (this.address !== '') {
        this.address.split(',').filter(Number).every(loco => {
          if (loco < 1 || loco > 10293) {
            target.setCustomValidity('A loco address must be a value between 1 & 10293.');
            target.reportValidity();
            return false;
          }

          target.setCustomValidity('');
          return true;
        });
      }
    },
    close() {
      this.address = '';
      this.locosMutated.forEach(loco => {
        loco.selected = false;
      });

      this.$emit('close');
    },
    select() {
      const addresses = this.locosMutated
        .filter(({ selected }) => selected)
        .map(({ address }) => address)
        .concat(this.address.split(',').filter(Boolean))
        .map(Number);

      this.$emit('select', addresses);
      this.close();
    }
  },
  template: `
  <div>
    <div class="modal d-block">
      <div class="modal-dialog m-0 m-md-auto">
        <form @submit.prevent="select" class="modal-content">
          <div class="modal-header">
            <h5 class="modal-title">Add Loco</h5>
            <button @click="close" type="button" class="btn-close"></button>
          </div>
          <div class="modal-body">
            <div class="row">
              <div class="col">
                <div class="form-floating">
                  <input v-model="address" @input="validAddress" type="text" class="form-control form-control-sm" inputmode="numeric" pattern="[\\d,]+" placeholder="Enter loco addresses, use a comma to provide multiple..." />
                  <label>Enter loco addresses, use a comma to provide multiple...</label>
                </div>
              </div>
            </div>
            <div class="row mt-3">
              <div class="col">
                <ul class="list-group" style="overflow-y: scroll; max-height: 288px;">
                  <li v-for="loco of locosMutated" :class="{ active: loco.selected }" @click="loco.selected = !loco.selected" class="list-group-item" style="cursor: pointer;">
                    <div class="row" style="pointer-events: none;">
                      <div class="col-2">
                        #{{ loco.address }}
                      </div>
                      <div class="col">
                        {{ loco.name }}
                      </div>
                    </div>
                  </li>
                </ul>
              </div>
            </div>
          </div>
          <div class="modal-footer">
            <button @click="close" type="button" class="btn btn-secondary">Close</button>
            <button type="submit" class="btn btn-primary">Select</button>
          </div>
        </form>
      </div>
    </div>
    <div class="modal-backdrop fade show"></div>
  </div>
  `
}

let dragSibling, oldRowIndex = null, oldBtnIndex = null;

export default {
  components: {
    Modal,
  },
  props: {
    active: Boolean
  },
  data() {
    return {
      add: false,
      groups: [],
      pending: false,
      locos: [],
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
  emits: ['lock', 'unlock'],
  methods: {
    lock() {
      this.$emit('lock');
    },
    unlock() {
      this.$emit('unlock');
    },
    async load() {
      this.isLoading = true;
      {
        const response = await fetch('/locos');
        const json = await response.json();
        this.locos = Object.fromEntries(json.map(({ name, file }) => {
          return [file.match(/\d+/)[0], name];
        }));
      }
      {
        const response = await fetch('/groups.json');
        if (response.ok) {
          this.groups = (await response.json()).map(group => {
            return Object.assign(group, {
                expand: false,
                key: rand(),
              });
          });
        } else {
          this.groups = [];
        }
      }
      this.isLoading = false;
    },
    addGroup() {
      this.groups.push({
        name: '',
        locos: [],
        expand: true,
        key: rand(),
      });
      this.lock();
    },
    delGroup(index) {
      if (confirm('Are you sure you want to delete this group?')) {
        this.groups.splice(index, 1);
        this.lock();
      }
    },
    addLoco(locos) {
      const dedup = locos.filter(loco => !this.groups[this.add].locos.includes(+loco));
      if (dedup.length) {
        this.groups[this.add].locos.push(...dedup);
        this.add = false;
        this.lock();
      }
    },
    delLoco(group, index) {
      if (confirm('Are you sure you want to delete this loco from the group?')) {
        this.groups[group].locos.splice(index, 1);
        this.lock();
      }
    },
    async save() {
      const response = await fetch('/groups.json', {
        method: 'PUT',
        headers: {
          'Content-Type': 'application/json'
        },
        body: JSON.stringify(this.groups.map(({ name, locos }) => ({ name, locos }))) // map to remove `expand` and `key`
      });

      if (response.ok) {
        this.unlock();
      }
    },
    discard() {
      this.load();
      this.unlock();
    },

    dragStart(event) {
      const existing = event.target.closest(`[draggable]`);
      dragSibling = existing.tagName.toLowerCase();

      if (document.elementFromPoint(event.x, event.y)?.matches('[role=button]')) {
        oldRowIndex = elementIndex(existing);
      } else if (existing.matches('.row')) {
        oldBtnIndex = elementIndex(existing);
        oldRowIndex = elementIndex(existing.closest('li'));
      } else {
        event.preventDefault();
      }

      event.dataTransfer.effectAllowed = 'move';
      event.dataTransfer.setData('text/plain', null);
    },
    dragOver(event) {
      const over = event.target.closest(`${dragSibling}[draggable]`);
      if (over) {
        if (dragSibling === 'li') {
          const newRowIndex = elementIndex(over);
          if (oldRowIndex !== newRowIndex) { // Has the row moved?
            const move = this.groups.splice(oldRowIndex, 1);
            this.groups.splice(newRowIndex, 0, ...move);

            oldRowIndex = newRowIndex;
          }
        } else if (dragSibling === 'div') {
          const newRowIndex = elementIndex(over.closest('li'));
          const newBtnIndex = elementIndex(over);
          if (oldRowIndex !== newRowIndex || (oldRowIndex === newRowIndex && oldBtnIndex !== newBtnIndex)) { // Has the btn moved?
            const move = this.groups[oldRowIndex].locos.splice(oldBtnIndex, 1);
            this.groups[newRowIndex].locos.splice(newBtnIndex, 0, ...move);

            oldRowIndex = newRowIndex;
            oldBtnIndex = newBtnIndex;
          }
        }
      }
    },
    dragEnd() {
      this.lock();
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
                Groups
              </h1>
              <span class="position-absolute top-0 start-100 translate-middle badge rounded-pill bg-primary mt-2">
                {{ groups.length }}
              </span>
            </div>
          </div>
          <div class="col-auto">
            <button @click="addGroup" class="btn btn-link text-success p-0">
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
        <ul @dragstart="dragStart" @dragover="dragOver" @dragend="dragEnd" class="list-group list-group-flush" :class="{ loading: isLoading }">
          <li v-for="({ locos: groupLocos, expand, key }, groupIndex) of groups" :key="key" class="list-group-item" draggable="true">
            <div class="row">
              <div class="col-auto" role="button">
                <svg width="16" height="16" fill="currentColor" style="pointer-events: none;">
                  <use xlink:href="bs.icons.svg#arrows-expand"/>
                </svg>
              </div>
              <div class="col-1 px-0 text-center">
                <span class="badge rounded-pill bg-primary">
                  {{ groupLocos.length }}
                </span>
              </div>
              <div class="col pe-0">
                <input v-model="groups[groupIndex].name" type="text" class="form-control form-control-sm" maxlength="20" required placeholder="Group Name" />
              </div>
              <div class="col-auto d-flex align-items-center ps-auto">
                <button @click="groups[groupIndex].expand = true" v-if="!expand" class="btn btn-link p-0 d-flex align-items-center">
                  <svg width="22" height="22" fill="currentColor">
                    <use xlink:href="bs.icons.svg#arrow-bar-down"/>
                  </svg>
                </button>
                <button @click="groups[groupIndex].expand = false" v-if="expand" class="btn btn-link p-0 d-flex align-items-center">
                  <svg width="22" height="22" fill="currentColor">
                    <use xlink:href="bs.icons.svg#arrow-bar-up"/>
                  </svg>
                </button>
                <button @click="add = groupIndex" class="btn btn-link text-success p-0">
                  <svg width="28" height="28" fill="currentColor">
                    <use xlink:href="bs.icons.svg#plus-lg"/>
                  </svg>
                </button>
                <button @click="delGroup(groupIndex)" class="btn btn-link p-0 d-flex align-items-center">
                  <svg width="16" height="16" fill="currentColor">
                    <use xlink:href="bs.icons.svg#trash"/>
                  </svg>
                </button>
              </div>
            </div>
            <div v-show="expand" class="mt-2">
              <div v-for="(loco, locoIndex) of groupLocos" :class="[!locos[loco]? 'alert-warning' : '']" :key="loco" class="row py-2" draggable="true" style="cursor: pointer;">
                <div class="col-auto">
                  <svg class="bi ms-2" width="16" height="16" fill="currentColor">
                    <use xlink:href="bs.icons.svg#arrow-return-right"/>
                  </svg>
                </div>
                <div class="col-3 col-md-2 ps-1">
                  #{{ loco }}
                </div>
                <div class="col px-0">
                  <span v-if="locos[loco]">
                    {{ locos[loco] }}
                  </span>
                  <span v-else>
                    Name Unknown
                  </span>
                </div>
                <div class="col-auto d-flex align-items-center ps-auto">
                  <button @click="delLoco(groupIndex, locoIndex)" class="btn btn-link p-0 d-flex align-items-center">
                    <svg width="16" height="16" fill="currentColor">
                      <use xlink:href="bs.icons.svg#trash"/>
                    </svg>
                  </button>
                </div>
              </div>
            </div>
          </li>
          <li v-if="!groups.length" class="list-group-item d-flex align-items-center">
            No groups, click the <span @click="addGroup" class="text-success h1" style="cursor: pointer;">+</span> to add one
          </li>
        </ul>
      </div>
    </div>
    <div class="row">
      <div class="col-md-5 mx-auto text-end">
        <hr class="mt-2 bg-secondary" />
        <button @click="discard" class="btn btn-secondary me-2">Discard Changes</button>
        <button @click="save" class="btn btn-primary">Save</button>
      </div>
    </div>
    <Modal v-show="add !== false" @close="add = false" @select="addLoco" :locos="locos" />
  </div>
  `
}
