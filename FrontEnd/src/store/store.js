import { createStore } from 'redux';
import reducers from './reducers'

const defaultState = {
  username: '',
  userId: '',
  page: 'login',
  obiews: [],
};

const store = createStore(reducers, defaultState);

export default store