import { rand, elementIndex } from './utils.js';

const Colors = {
  'rgb(0, 0, 0)':       0x0000, // black
  'rgb(0, 0, 128)':     0x000F, // navy
  'rgb(0, 128, 0)':     0x03E0, // dark green
  'rgb(0, 128, 128)':   0x03EF, // dark cyan
  'rgb(128, 0, 0)':     0x7800, // maroon
  'rgb(128, 0, 128)':   0x780F, // purple
  'rgb(128, 128, 0)':   0x7BE0, // olive
  'rgb(211, 211, 211)': 0xD69A, // light grey
  'rgb(128, 128, 128)': 0x7BEF, // dark grey
  'rgb(0, 0, 255)':     0x001F, // blue
  'rgb(0, 255, 0)':     0x07E0, // green
  'rgb(0, 255, 255)':   0x07FF, // cyan
  'rgb(255, 0, 0)':     0xF800, // red
  'rgb(255, 0, 255)':   0xF81F, // magenta
  'rgb(255, 255, 0)':   0xFFE0, // yellow
  'rgb(255, 255, 255)': 0xFFFF, // white
  'rgb(255, 180, 0)':   0xFDA0, // orange
  'rgb(180, 255, 0)':   0xB7E0, // green yellow
  'rgb(255, 192, 203)': 0xFE19, // pink
  'rgb(150, 75, 0)':    0x9A60, // brown
  'rgb(255, 215, 0)':   0xFEA0, // gold
  'rgb(192, 192, 192)': 0xC618, // silver
  'rgb(135, 206, 235)': 0x867D, // sky blue
  'rgb(180, 46, 226)':  0x915C, // violet
};

const decToRgb = dec => Object.keys(Colors).find(key => Colors[key] === dec);

const ColorPicker = {
  data() {
    return {
      modal: false,
    }
  },
  props: ['modelValue'],
  emits: ['update:modelValue'],
  computed: {
    colors() {
      const colors = Object.keys(Colors);
      const rows = [];

      for (let i = 0; i < colors.length; i++) {
        if (i % 4 === 0) {
          rows.push([]);
        }
        rows.at(-1).push(colors[i]);
      }

      return rows;
    },
    rgb() {
      return decToRgb(this.modelValue);
    }
  },
  methods: {
    select(color) {
      this.modal = false;
      this.$emit('update:modelValue', Colors[color]);
    }
  },
  template: `
  <div @click="modal = true" :style="{ '--rgb': rgb }" class="color-picker"></div>
  <div v-if="modal" class="modal d-block">
    <div class="modal-dialog modal-sm modal-dialog-centered">
      <div class="modal-content">
        <div class="modal-body">
          <div class="container-fluid px-2">
            <div v-for="row of colors" class="row">
              <div v-for="color of row" class="col" :style="{ 'background-color': color }" @click="select(color)" style="height: 30px; cursor: pointer;"></div>
            </div>
          </div>
        </div>
        <div class="modal-footer">
          <button @click="modal = false" type="button" class="btn btn-secondary">Cancel</button>
        </div>
      </div>
    </div>
  </div>
  `
};

const FnIcon = {
  data() {
    return {
      icons: [],
    };
  },
  props: ['modelValue'],
  emits: ['update:modelValue'],
  methods: {
    next(index) { // Lazy load icons to not overload the ESP32
      if (this.icons[index]) {
        this.icons[index].load = true;
      }
    },
    toggle(file) {
      if (file === this.modelValue) {
        file = null;
      }
      this.$emit('update:modelValue', file);
    }
  },
  async mounted() {
    const response = await fetch('/icons');
    this.icons = (await response.json()).map((icon, idx) => {
      return {
        file: icon,
        load: idx === 0 || this.icons.findIndex(({ file }) => file === icon) !== -1
      }
    });
  },
  template: `
  <div class="fn-icons">
    <div class="position-absolute">
      <div v-for="({ file, load }, idx) of icons" :key="file"
        :class="{ 'border-primary': modelValue == file, 'border-white': modelValue != file }"
        @click="toggle(file)"
        class="border border-5 d-inline-block rounded p-1"
        role="button"
      >
        <img :src="load ? file : null" :data-src="file" @load="next(idx + 1)" />
      </div>
    </div>
  </div>
  `
};

const FnButton = {
  components: {
    ColorPicker,
    FnIcon,
  },
  data() {
    return {
      active: false,
      advanced: false,
      activeStyle: 'idle',

      config: {
        fn: null,
        latching: null,
        btn: {
          idle: {
            label: null,
            color: null,
            fill: null,
            border: null,
            icon: null
          },
          pressed: {
            label: null,
            color: null,
            fill: null,
            border: null,
            icon: null
          }
        }
      }
    }
  },
  props: ['modelValue', 'activate'],
  emits: ['update:modelValue', 'activate'],
  watch: {
    activate(value) {
      this.active = this.$el === value;
      this.advanced = false;
      this.activeStyle = 'idle';
      this.load();
    },
    'config.fn': function(value) {
      if (this.config.btn.idle.label.match(/F\d*/i) && this.config.btn.pressed.label.match(/F\d*/i)) {
        this.config.btn.idle.label = this.config.btn.pressed.label = `F${value}`;
      }
    }
  },
  computed: {
    style() {
      const active = this.config.btn[this.activeStyle];

      return Object.assign({ }, active, { // Change decimal colours to rgb for preview
        color: decToRgb(active.color),
        fill: decToRgb(active.fill),
        border: decToRgb(active.border)
      });
    }
  },
  methods: {
    load() {
      const stringified = JSON.stringify(this.modelValue);
      if (JSON.stringify(this.config) !== stringified) {
        this.config = JSON.parse(stringified);
      }
    },
    apply() {
      this.$emit('update:modelValue', JSON.parse(JSON.stringify(this.config)));
      this.$emit('activate', null);
    },
    cancel() {
      this.load();
      this.$emit('activate', null);
    },
    preview() {
      if (this.activeStyle === 'idle') {
        this.activeStyle = 'pressed';
      } else {
        this.activeStyle = 'idle';
      }
    }
  },
  mounted() {
    this.load();
  },
  template: `
  <div :class="{ active: active }" class="fn-button">
    <button @click="this.$emit('activate', this.active ? null : this.$el)"
      :style="{ color: style.color, 'background-color': style.fill, 'border-color': style.border }"
      type="button" class="btn w-100 p-0 d-flex align-items-center justify-content-center" role="button"
    >
      <img v-if="style.icon" :src="style.icon" />
      <span class="ms-1" v-if="style.label != ''">{{ style.label }}</span>
    </button>
    <div v-if="active" :style="advanced ? 'height: 248px;' : 'height: 46px;'" class="mt-2">
      <div class="position-relative">
        <div></div>
      </div>
      <div class="position-absolute w-100 p-2 bg-white rounded" style="left: 0;">
        <div @click="preview" class="preview">
          <svg width="16" height="16" fill="currentColor">
            <use xlink:href="bs.icons.svg#toggles"/>
          </svg>
        </div>
        <div class="row">
          <div class="col-auto">
            <svg @click="advanced = !advanced" width="32" height="32" fill="currentColor" role="button">
              <use xlink:href="bs.icons.svg#menu-app"/>
            </svg>
          </div>
          <div class="col">
            <input v-model="config.fn" type="number" class="form-control form-control-sm d-inline-block me-2" min="0" max="28" style="width: 50px;" />
            <div class="form-check form-switch d-inline-block align-middle">
              <input v-model="config.latching" class="form-check-input" type="checkbox" role="switch" checked>
            </div>
          </div>
          <div class="col-auto">
            <svg @click="apply" class="text-success" width="32" height="32" fill="currentColor" role="button">
              <use xlink:href="bs.icons.svg#check"/>
            </svg>
            <svg @click="cancel" class="text-danger" width="32" height="32" fill="currentColor" role="button">
              <use xlink:href="bs.icons.svg#x"/>
            </svg>
          </div>
        </div>
        <div v-if="advanced" class="row text-center">
          <div class="col">
            <h5>Idle</h5>
            <input v-model="config.btn.idle.label" class="form-control form-control-sm text-center">
            <div class="row mt-2">
              <div class="col p-0">
                Txt<br /><ColorPicker v-model="config.btn.idle.color" />
              </div>
              <div class="col p-0">
                Fill<br /><ColorPicker v-model="config.btn.idle.fill" />
              </div>
              <div class="col p-0">
                Bdr<br /><ColorPicker v-model="config.btn.idle.border" />
              </div>
            </div>
            <div class="row mt-2">
              <div class="col">
                <FnIcon v-model="config.btn.idle.icon" />
              </div>
            </div>
          </div>
          <div class="col">
            <h5>Pressed</h5>
            <input v-model="config.btn.pressed.label" class="form-control form-control-sm text-center">
            <div class="row mt-2">
              <div class="col p-0">
                Txt<br /><ColorPicker v-model="config.btn.pressed.color" />
              </div>
              <div class="col p-0">
                Fill<br /><ColorPicker v-model="config.btn.pressed.fill" />
              </div>
              <div class="col p-0">
                Bdr<br /><ColorPicker v-model="config.btn.pressed.border" />
              </div>
            </div>
            <div class="row mt-2">
              <div class="col">
                <FnIcon v-model="config.btn.pressed.icon" />
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
  `
};

let isNew = false, dragSibling, oldRowIndex = null, oldBtnIndex = null;

export default {
  components: {
    FnButton,
  },
  data() {
    return {
      mutatedModel: [],
      activated: null,
    }
  },
  props: {
    modelValue: Array,
  },
  emits: ['update:modelValue'],
  watch: {
    modelValue(value) {
      if (!value.length && this.mutatedModel.length) {
        this.mutatedModel = [];
      } else if (!this.mutatedModel.length) { // Add random keys to the rows and buttons if they don't already have one
        this.mutatedModel = value.map(row => {
          row.forEach(btn => {
            if (typeof btn === 'object' && !btn.key) {
              btn.key = rand();
            }
          });

          if (!Number.isInteger(row.at(-1))) {
            row.push(rand());
          }

          return row;
        });
      }
    },
    mutatedModel: {
      handler(value) {
        const update = this.removeKeys(JSON.parse(JSON.stringify(value)));
        if (JSON.stringify(update) !== JSON.stringify(this.modelValue)) {
          this.$emit('update:modelValue', update);
        }
      },
      deep: true,
    }
  },
  methods: {
    // Remove the keys needed by VueJS for state
    removeKeys(fns) {
      return fns.map(row => row.filter(btn => typeof btn === 'object').map(btn => {
        delete btn.key;
        return btn; 
      }));
    },
    dragStart(event) {
      if (this.$el.querySelectorAll('.active').length) { // No drag if a button is active
        event.preventDefault();
        return;
      }

      if (event.target.matches('[data-type="row"]')) { // Drag in new row
        isNew = true;
        dragSibling = 'row';
      } else if (event.target.matches('[data-type="col"]')) { // Drag in new button
        isNew = true;
        dragSibling = 'col';
      } else { // Drag existing row or button
        const existing = event.target.closest(`[draggable]`);
        if (existing.matches('.row')) {
          dragSibling = 'row';
          oldRowIndex = elementIndex(existing);
        } else if (existing.matches('.col')) {
          dragSibling = 'col';
          oldRowIndex = elementIndex(existing.closest('.row'));
          oldBtnIndex = elementIndex(existing);
        }
      }

      event.dataTransfer.effectAllowed = 'move';
      event.dataTransfer.setData('text/plain', null);
    },
    dragOver({ target }) {
      if (isNew) {
        if (dragSibling === 'row' && (target == this.$refs.functions || target.closest('.row[draggable]'))) { // New row
          isNew = false;
          oldRowIndex = this.mutatedModel.length;
          this.mutatedModel.splice(oldRowIndex, 0, [rand()]);
        } else if (dragSibling === 'col' && target.closest('[draggable]')) { // New btn
          const row = target.closest('.row[draggable]');
          if (row && row.childElementCount < 4) { // New btn only allowed if there isn't already 4
            isNew = false;
            oldRowIndex = elementIndex(row);
            oldBtnIndex = row.childElementCount;

            // Get the next available fn number
            const nextFn = Math.min(Math.max(-1, ...this.mutatedModel.map(row => Math.max(...row.slice(0, -1).map(btn => btn.fn)))) + 1, 28);

            this.mutatedModel[oldRowIndex].splice(oldBtnIndex, 0, { // Default config for new button
              key: rand(),
              fn: nextFn,
              latching: true,
              btn: {
                idle: {
                  label: `F${nextFn}`,
                  color: 65535,
                  fill: 0,
                  border: 65535,
                  icon: ""
                },
                pressed: {
                  label: `F${nextFn}`,
                  color: 0,
                  fill: 65535,
                  border: 65535,
                  icon: ""
                }
              }
            });
          }
        }
      } else if (dragSibling === 'row') { // Move row
        const sibling = target.closest('.row[draggable]');
        if (sibling) {
          const newRowIndex = elementIndex(sibling);
          if (oldRowIndex !== newRowIndex) { // Has the row moved?
            const move = this.mutatedModel.splice(oldRowIndex, 1);
            this.mutatedModel.splice(newRowIndex, 0, ...move);

            oldRowIndex = newRowIndex;
          }
        }
      } else if (dragSibling === 'col') { // Move col/btn
        const sibling = target.closest(`.col[draggable],.row[draggable]:empty`);
        if (sibling) {
          if (sibling.matches('.row')) { // Empty row
            const newRowIndex = elementIndex(sibling);
            const newBtnIndex = 0;

            const move = this.mutatedModel[oldRowIndex].splice(oldBtnIndex, 1);
            this.mutatedModel[newRowIndex].splice(newBtnIndex, 0, ...move);

            oldRowIndex = newRowIndex;
            oldBtnIndex = newBtnIndex;
          } else if (sibling.parentNode.childElementCount < 4 || (sibling.parentNode.childElementCount === 4 && elementIndex(sibling.parentNode) === oldRowIndex)) { // Move to new row or within current row
            const newRowIndex = elementIndex(sibling.parentNode);
            const newBtnIndex = elementIndex(sibling);
            if (oldRowIndex !== newRowIndex || (oldRowIndex === newRowIndex && oldBtnIndex !== newBtnIndex)) {
              const move = this.mutatedModel[oldRowIndex].splice(oldBtnIndex, 1);
              this.mutatedModel[newRowIndex].splice(newBtnIndex, 0, ...move);

              oldRowIndex = newRowIndex;
              oldBtnIndex = newBtnIndex;
            }
          }
        }
      }
    },
    del() {
      if (oldRowIndex !== null) {
        if (oldBtnIndex === null) { // Delete row
          this.mutatedModel.splice(oldRowIndex, 1);
        } else {
          this.mutatedModel[oldRowIndex].splice(oldBtnIndex, 1); // Delete btn
        }

        oldRowIndex = null;
        oldBtnIndex = null;
      }
    },
    updateBtn(row, btn, change) {
      this.mutatedModel[row][btn] = change;
    },
  },
  template: `
  <div @dragstart="dragStart" @dragover="dragOver">
    <div ref="functions" class="d-inline-block position-relative pb-2 fn-grid">
      <div v-for="(row, rowIndex) of mutatedModel" :key="row.at(-1)" class="row position-relative mx-2 mt-2" draggable="true">
        <FnButton v-for="(btn, btnIndex) of row.slice(0, -1)" :key="btn.key" v-model="btn"
          @update:modelValue="value => updateBtn(rowIndex, btnIndex, value)"
          @activate="value => this.activated = value" :activate="activated"
          class="col ps-0" draggable="true"
        />
      </div>
    </div>
    <div class="mt-3">
      <button data-type="row" type="button" class="btn btn-primary mx-1" draggable="true">
        <svg width="16" height="16" fill="currentColor">
          <use xlink:href="bs.icons.svg#arrow-left-right"/>
        </svg>
      </button>
      <button data-type="col" type="button" class="btn btn-primary mx-1" draggable="true">
        <svg width="16" height="16" fill="currentColor">
          <use xlink:href="bs.icons.svg#menu-button"/>
        </svg>
      </button>
      <button @dragover.prevent @drop="del" type="button" class="btn btn-primary mx-1">
        <svg width="16" height="16" fill="currentColor">
          <use xlink:href="bs.icons.svg#trash"/>
        </svg>
      </button>
    </div>
  </div>
  `
}
