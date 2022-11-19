export default {
  props: {
    active: Boolean
  },
  data() {
    return {
      ssid: '',
      password: '',
      server: '',
      port: 0,
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
  emits: ['lock', 'unlock', 'valid'],
  methods: {
    lock() {
      this.$emit('lock');
    },
    unlock(valid = null) {
      this.$emit('unlock', valid);
    },
    async load() {
      const response = await fetch('/cs');
      if (response.ok) {
        ({
          ssid: this.ssid,
          password: this.password,
          server: this.server,
          port: this.port
        } = await response.json());
      }
    },
    async save() {
      const response = await fetch('/cs', {
        method: 'PUT',
        headers: {
          'Content-Type': 'application/json'
        },
        body: JSON.stringify({
          ssid: this.ssid,
          password: this.password,
          server: this.server,
          port: this.port
        })
      });

      if (response.ok) {
        this.unlock(true);
      }
    },
    discard() {
      this.load();
      this.unlock();
    },
  },
  template: `
  <form @submit.prevent="save">
    <div class="row">
      <div class="col-md-5 mx-auto">
        <h1 class="display-5">
          CS Settings
        </h1>
      </div>
    </div>
    <div class="row">
      <div class="col-md-5 mx-auto">
        <hr class="bg-secondary" />
      </div>
    </div>
    <div class="row">
      <div class="col-md-4 mx-auto">
        <div class="mb-3 row">
          <div class="col pe-0">
            <div class="form-floating">
              <input v-model="ssid" @input="lock" type="text" class="form-control" required placeholder="WiFi SSID">
              <label>WiFi SSID</label>
            </div>
          </div>
          <div class="col">
            <div class="form-floating">
              <input v-model="password" @input="lock" type="text" class="form-control" placeholder="WiFi Password" />
              <label>WiFi Password</label>
            </div>
          </div>
        </div>
        <div class="mb-2 row">
          <div class="col pe-0">
            <div class="form-floating">
              <input v-model="server" @input="lock" type="text" class="form-control" required placeholder="CS Server Host/IP" />
              <label>CS Server Host/IP</label>
            </div>
          </div>
          <div class="col">
            <div class="form-floating">
              <input v-model="port" @input="lock" type="text" class="form-control" required placeholder="CS Server Port" />
              <label>CS Server Port</label>
            </div>
          </div>
        </div>
      </div>
    </div>
    <div class="row">
      <div class="col-md-5 mx-auto text-end">
        <hr class="mt-2 bg-secondary" />
        <button @click="discard" class="btn btn-secondary me-2">Discard Changes</button>
        <button type="submit" class="btn btn-primary">Save</button>
      </div>
    </div>
    <div class="row mt-3">
      <div class="col-md-5 mx-auto">
        <div class="card">
          <div class="card-header">
            <svg width="16" height="16" fill="currentColor">
              <use xlink:href="bs.icons.svg#info"/>
            </svg>
            Note
          </div>
          <div class="card-body">
            <ul class="list-group list-group-flush">
              <li class="list-group-item">If your CS connects to your home WiFi the defaults for Server Host/IP &amp; Port are 
                <span class="badge text-bg-secondary">dccex</span> and <span class="badge text-bg-secondary">2560</span></li>
              <li class="list-group-item">If your CS is in AP mode the defaults for Server Host/IP &amp; Port are 
                <span class="badge text-bg-secondary">192.168.4.1</span> and <span class="badge text-bg-secondary">2560</span></li>
            </ul>
          </div>
        </div>
      </div>
    </div>
  </form>
  `
}
