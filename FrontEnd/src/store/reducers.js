import constants from './constants'

const reducers = (state={}, action) => {
  switch(action.type) {
    case constants.CHANGE_PAGE: {
      return {
        ...state,
        page: action.page,
      }
    }
    case constants.CHANGE_USER: {
      return {
        ...state,
        username: action.username,
        userId: action.userId,
      }
    }
    case constants.CHANGE_OBIEWS: {
      return {
        ...state,
        obiews: action.obiews,
      }
    }
    case constants.CREATE_NEW_OBIEW: {
      let obiews = state.obiews.slice(0)
      obiews.unshift(action.obiew)
      return {
        ...state,
        obiews: obiews
      }
    }
    case constants.CREATE_NEW_COMMENT: {
      let obiews = state.obiews.slice(0).map(obiew => {
        if (obiew.obiewId === action.obiewId) {
          obiew.comments.unshift(action.comment);
        }
        return obiew;
      })
      return {
        ...state,
        obiews: obiews
      }
    }
    case constants.CREATE_NEW_LIKE: {
      let obiews = state.obiews.slice(0).map(obiew => {
        if (obiew.obiewId === action.obiewId) {
          obiew.likes.push(action.like);
        }
        return obiew;
      })
      return {
        ...state,
        obiews: obiews
      }
    }
    case constants.DELETE_LIKE: {
      let obiews = state.obiews.slice(0).map(obiew => {
        if (obiew.obiewId === action.obiewId) {
          obiew.likes = obiew.likes.filter(like => like.userId !== action.userId);
        }
        return obiew;
      })
      return {
        ...state,
        obiews: obiews
      }
    }
    default:
      return state
  }
}

export default reducers;