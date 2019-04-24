import constants from './constants'
import { combineReducers } from 'redux'

const reducers = (state={}, action) => {
  switch(action.type) {
    case constants.CHANGE_USER:
      return {
        ...state,
        userId: action.username
      }
    case constants.CREATE_NEW_OBIEW:
      let obiews = state.obiews.slice(0)
      obiews.unshift(action.obiew)
      return {
        ...state,
        obiews: obiews
      }
    case constants.CREATE_NEW_COMMENT:
      obiews = state.obiews.slice(0).map(obiew => {
        if (obiew.obiewId === action.obiewId) {
          obiew.comments.unshift(action.comment);
        }
        return obiew;
      })
      return {
        ...state,
        obiews: obiews
      }
    case constants.CREATE_NEW_LIKE:
      obiews = state.obiews.slice(0).map(obiew => {
        if (obiew.obiewId === action.obiewId) {
          obiew.likes.push(action.like);
        }
        return obiew;
      })
      return {
        ...state,
        obiews: obiews
      }
    case constants.DELETE_LIKE:
      obiews = state.obiews.slice(0).map(obiew => {
        if (obiew.obiewId === action.obiewId) {
          obiew.likes.filter(like => like.likeId !== action.likeId);
        }
        return obiew;
      })
      return {
        ...state,
        obiews: obiews
      }
    default:
      return state
  }
}

export default combineReducers({
  reducers,
});