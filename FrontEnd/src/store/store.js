import { createStore } from 'redux';
import reducers from './reducers'

const defaultState = {
  username: '',
  page: 'register',
  obiews: [
    {
      userId: 0,
      obiewId: 0,
      pic: "../../images/person_default.png",
      username: "Anonymous",
      timestamp: Date.now(),
      hasReobiewed: false,
      likes: [
        {
          userId: 1,
          obiewId: 0
        },
        {
          userId: 2,
          obiewId: 0
        },
        {
          userId: 3,
          obiewId: 0
        },
      ], 
      comments: [
        {
          obiewId: 0,
          userId: 1,
          timestamp: Date.now(),
          reply: "Words cannot describe the tremendous sorrow for the great loss of the innocents."
        },
        {
          obiewId: 0,
          userId: 2,
          timestamp: Date.now(),
          reply: "Words cannot describe the tremendous sorrow for the great loss of the innocents."
        }
      ],
      reobiews: [
        {
          userId: 1,
          obiewId: 1,
          timestamp: Date.now(),
        }
      ],
      obiew: "The number of people who died in the Easter Sunday attacks in Sri Lanka has risen sharply to 290, police say."
    },
    {
      userId: 2,
      obiewId: 1,
      pic: "../../images/person_default.png",
      username: "Anonymous",
      timestamp: Date.now(),
      likes: [
        {
            userId: 1,
            obiewId: 1,
        }
      ], 
      hasReobiewed: false,
      comments: [
        {
          obiewId: 1,
          userId: 1,
          timestamp: Date.now(),
          reply: "Words cannot describe the tremendous sorrow for the great loss of the innocents."
        }
      ], 
      reobiews: [],
      obiew: "The number of people who died in the Easter Sunday attacks in Sri Lanka has risen sharply to 290, police say."
    }
  ],
};

const store = createStore(reducers, defaultState);

export default store